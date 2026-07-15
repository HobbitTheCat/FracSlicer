#include "io/fractal_import.h"
#include "core/geometry_types.h"

#include <queue>
#include <unordered_set>

namespace json_import {

const std::size_t INVALID_FACE = std::numeric_limits<std::size_t>::max();

FractalModel import_json(const std::string &file_name) {
    std::ifstream file(file_name);
    if (!file.is_open()) throw std::runtime_error("Json Import: Cannot open file" + file_name);

    nlohmann::json root;
    file >> root;

    // Searching for initial '#' state
    int initial_state_id = -1;
    std::unordered_map<int, nlohmann::json> states_map;
    
    for (const auto& state_json : root["states"]) {
        int id = state_json["id"];
        states_map[id] = state_json;
        if (state_json["name"] == "#") {
            initial_state_id = id;
        }
    }
    if (initial_state_id == -1) throw std::runtime_error("Json Import: Macro state '#' not found");

    // Mapping all transitions
    std::unordered_map<int, std::vector<nlohmann::json>> subdivision_transitions;
    std::vector<nlohmann::json> root_transitions;

    for (const auto& trans_json : root["transitions"]) {
        if (trans_json["type"] != "SUBDIVISION") continue;

        int from_id = trans_json["from_state_id"];
        if (from_id == initial_state_id) {
            root_transitions.push_back(trans_json);
        } else {
            subdivision_transitions[from_id].push_back(trans_json);
        }
    }

    if (root_transitions.empty()) throw std::runtime_error("Json Import: No SUBDIVISION transitions from initial state '#'");

    std::vector<MeshInstance> initial_instances;
    std::queue<int> queue;
    std::unordered_set<int> visited_ids;

    for (const auto& root_trans : root_transitions) {
        std::size_t rows = root_trans["matrix_rows"];
        std::size_t cols = root_trans["matrix_cols"];
        arma::mat T(rows, cols, arma::fill::zeros);
        const auto& mat_data = root_trans["matrix"];

        for (std::size_t r = 0; r < rows; r++) {
            for (std::size_t c = 0; c < cols; c++) {
                T(r, c) = mat_data[r][c];
            }
        }

        MeshInstance instance;
        instance.control_points = T.t();

        int target_state_id = root_trans["to_state_id"];
        instance.automaton_state = target_state_id;
        initial_instances.push_back(std::move(instance));

        if (visited_ids.find(target_state_id) == visited_ids.end()) {
            visited_ids.insert(target_state_id);
            queue.push(target_state_id);
        }
    }

    int max_state_id = 0;
    for (const auto& [id, _] : states_map) {
        if (id > max_state_id) {
            max_state_id = id;
        }
    }

    Automaton automaton;
    automaton.resize_for_max_id(max_state_id);

    while(!queue.empty()) {
        int current_id = queue.front();
        queue.pop();

        const auto& state_json = states_map[current_id];
        MeshTemplate ref_topology = parse_primitive_state(state_json);
        
        // #ifndef MY_DEBUG_FLAG
        validate_mesh(ref_topology, state_json["name"]);
        // #endif
        
        std::vector<Automaton::StateTransition> current_transitions;
                
        for (const auto& trans_json : subdivision_transitions[current_id]) {
            int target_id = trans_json["to_state_id"];
            
            std::size_t rows = trans_json["matrix_rows"];
            std::size_t cols = trans_json["matrix_cols"];
            arma::mat T(rows, cols, arma::fill::zeros);

            const auto& mat_data = trans_json["matrix"];
            for (std::size_t r = 0; r < rows; r++) {
                for (std::size_t c = 0; c < cols; c++) {
                    T(r, c) = mat_data[r][c];
                }
            }

            current_transitions.push_back(Automaton::StateTransition{T.t(), static_cast<std::size_t>(target_id)});

            if (visited_ids.find(target_id) == visited_ids.end()) {
                visited_ids.insert(target_id);
                queue.push(target_id);
            }
        }

        automaton.set_state(current_id, std::move(current_transitions), std::move(ref_topology));
    }

    return FractalModel(std::move(automaton), std::move(initial_instances));
}

// ART  It is worth noting that when parsing, I expect the vertices of each face to be listed in counterclockwise order
//      and that the same vertex will have the same set of barycentric coordinates

MeshTemplate parse_primitive_state(const nlohmann::json &state_json) {
    MeshTemplate mesh;

    if (!state_json["has_primitive"] || !state_json.contains("primitive_faces")) return mesh;

    std::vector<std::vector<double>> unique_vertices;
    std::map<std::pair<std::size_t, std::size_t>, std::size_t> edge_map;
    std::vector<arma::uword> temp_polygons;

    const auto& faces_json = state_json["primitive_faces"];

    for (std::size_t face_idx = 0; face_idx < faces_json.size(); face_idx++) {
        mesh.face_offsets.push_back(mesh.face_edges.size()); // Added an offset for the new face

        const auto& face = faces_json[face_idx];        // dict
        if (!face.contains("vertices") || face["vertices"].empty()) continue;
        std::vector<std::size_t> current_face_v_indices; // array of vertex ids for current face

        for (const auto& v_json : face["vertices"]) {  // array
            std::vector<double> coords = v_json.get<std::vector<double>>();
            std::size_t v_idx = INVALID_FACE;

            for (std::size_t i = 0; i < unique_vertices.size(); i++) {
                bool match = true;
                for (std::size_t j = 0; j < coords.size(); j++) {
                    if (std::abs(unique_vertices[i][j] - coords[j]) > 1e-6) {match = false; break;} // ART maybe this comparison doesn't make sense
                }
                if (match) {v_idx = i; break;}
            }

            // If this vertex is not among the known ones, we add it
            if (v_idx == INVALID_FACE) {
                v_idx = unique_vertices.size();
                unique_vertices.push_back(coords);
            }

            current_face_v_indices.push_back(v_idx);
        }

        std::size_t num_face_vertices = current_face_v_indices.size(); // number of vertices per edge

        for (std::size_t i = 0; i < num_face_vertices; i++) {
            std::size_t v_a = current_face_v_indices[i];
            std::size_t v_b = current_face_v_indices[(i + 1) % num_face_vertices];

            std::size_t v_min = std::min(v_a, v_b);
            std::size_t v_max = std::max(v_a, v_b);
            auto edge_key = std::make_pair(v_min, v_max);

            std::size_t current_edge_idx;

            if (edge_map.find(edge_key) == edge_map.end()) {
                current_edge_idx = mesh.edges.size();
                edge_map[edge_key] = current_edge_idx;
                mesh.edges.push_back({v_min, v_max, face_idx, INVALID_FACE});
            } else {
                current_edge_idx = edge_map[edge_key];
                mesh.edges[current_edge_idx].f1 = face_idx;
            }
            mesh.face_edges.push_back(current_edge_idx);
        }

        if (num_face_vertices >= 3) {
            for (std::size_t i = 1; i < num_face_vertices - 1; i++) {      // ART uses a standard algorithm for creating polygons here  
                temp_polygons.push_back(current_face_v_indices[0]);        // Just set one point and iterate through the rest, creating a polygon with the zero point and every other point.
                temp_polygons.push_back(current_face_v_indices[i]);        // Does not work for non-convex shapes; smooths non-coplanar shapes
                temp_polygons.push_back(current_face_v_indices[i + 1]);
            } 
        }
    }

    mesh.face_offsets.push_back(mesh.face_edges.size());

    if (!unique_vertices.empty()) {
        std::size_t n_rows = unique_vertices.size();
        std::size_t n_cols = unique_vertices[0].size(); // TODO: Check that everything works here without `barycentric_dim`

        mesh.barycentric_coords.set_size(n_rows, n_cols);
        for (std::size_t i = 0; i < n_rows; i++) {
            for (std::size_t j = 0; j < n_cols; j++) {
                mesh.barycentric_coords(i, j) = unique_vertices[i][j];
            }
        }
        mesh.polygons = arma::uvec(temp_polygons);
    }

    return mesh;
}

bool validate_mesh(const MeshTemplate &mesh, const std::string& primitive_name) {
    std::size_t V = mesh.barycentric_coords.n_rows;
    std::size_t E = mesh.edges.size();
    std::size_t F = mesh.face_offsets.size() - 1;

    std::cout << "Validating primitive: " << primitive_name << "\n";
    std::cout << "Topology count: Vertices = " << V << ", Edges = " << E << ", Faces = " << F << "\n";

    bool is_valid = true;

    for (std::size_t i = 0; i < E; i++) {
        const auto& edge = mesh.edges[i];
        if (edge.f0 == INVALID_FACE || edge.f1 == INVALID_FACE) {
            std::cerr << "[ERROR] Edge " << i << "(Vertices: " << edge.v0 << " -> " << edge.v1
                      << ") is open. Adjacency: f0= " << edge.f0 << ", f1 = " << edge.f1 << "\n";
            is_valid = false;
        }
    }

    std::size_t total_edges_in_faces = 0;
    for (std::size_t f = 0; f < F; f++) {
        std::size_t start = mesh.face_offsets[f];
        std::size_t end = mesh.face_offsets[f + 1];
        std::size_t face_edge_count = end - start;

        if (face_edge_count < 3) {
            std::cerr << "[ERROR] Face " << f << " has less than 3 edges. (Count: " << face_edge_count << ")\n";
            is_valid = false;
        }
        total_edges_in_faces += face_edge_count;
    }

    if (total_edges_in_faces != mesh.face_edges.size()) {
        std::cerr << "[ERROR] CSR mismatch: sum of face edges (" << total_edges_in_faces
                  << ") doesn't match face_edges array size (" << mesh.face_edges.size() << ")\n";
        is_valid = false;
    }

    int euler = static_cast<int>(V) - static_cast<int>(E) + static_cast<int>(F);
    if (euler != 2) {
        std::cerr << "[WARNING/ERROR] Euler characteristic V - E + F is" << euler << "\n";
        is_valid = false;
    }

    if (is_valid) std::cout << "[SUCCESS] Mesh topology is valid\n";
    else std::cout << "[FAIL] Mesh topology has errors\n";
    return is_valid;
}
    
} //namespace json_import