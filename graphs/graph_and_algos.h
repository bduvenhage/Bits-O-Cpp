class Graph
{
public:
    Graph() : num_edges_(0) {}
    
    ALWAYS_INLINE int get_num_vertices() const {return adjacency_list_.size();}
    ALWAYS_INLINE int get_num_edges() const {return num_edges_;}
    ALWAYS_INLINE int get_degree(const int v) const {return adjacency_list_[v].size();}
    ALWAYS_INLINE int get_max_degree(const int v) const
    {
        int max_degree = 0;
        for (int v=0; v<adjacency_list_.size(); ++v)
        {
            const int degree = get_degree(v);
            if (degree > max_degree) max_degree = degree;
        }
        return max_degree;
    }
    ALWAYS_INLINE int get_avrg_degree(const int v) const {return 2*get_num_edges() / get_num_vertices();}
    const std::vector<int> &get_adjacent_vertices(const int v) const {return adjacency_list_[v];}
    
    void add_edge(const int v, const int w)
    {
        const int num_vertices = std::max(v, w) + 1;
        if (num_vertices > adjacency_list_.size())
        {
            adjacency_list_.resize(num_vertices);
        }
        adjacency_list_[v].push_back(w);
        adjacency_list_[w].push_back(v);
        num_edges_+=1;
    }
    
private:
    int num_edges_;
    std::vector<std::vector<int>> adjacency_list_;
};


class DFS
{
public:
    DFS(const Graph &graph, const int start_vertex) :
    graph_(graph), num_vertices_(graph.get_num_vertices()), start_vertex_(start_vertex)
    {
        vertex_sequence_.reserve(num_vertices_);
        vertex_flags_.resize(num_vertices_, 0);
        edge_to_.resize(num_vertices_, 0);
        depth_to_.resize(num_vertices_, 0);
        
        dfs(start_vertex_);
    }
    
    const std::vector<int> &get_vertex_sequence()
    {
        return vertex_sequence_;
    }
    
    const std::vector<int> &get_edge_to()
    {
        return edge_to_;
    }
    
    const std::vector<int> &get_depth_to()
    {
        return depth_to_;
    }
    
    bool has_path_to(const int w) const
    {
        return (vertex_flags_[w] == 1);
    }
    
    std::vector<int> get_path_to(const int w)
    {
        std::vector<int> path;
        int depth = depth_to_[w];
        int v = w;
        path.resize(depth+1, 0);
        
        for (; depth>=0; --depth)
        {
            path[depth] = v;
            v = edge_to_[v];
        }
        
        return path;
    }
    
private:
    void dfs(const int v)
    {
        vertex_sequence_.push_back(v);
        vertex_flags_[v] = 1;
        
        const std::vector<int> &adj = graph_.get_adjacent_vertices(v);
        
        for (int j=0; j<adj.size(); ++j)
        {
            const int w = adj[j];
            if (vertex_flags_[w] == 0)
            {
                edge_to_[w] = v;
                depth_to_[w] = depth_to_[v] + 1;
                dfs(w);
            }
        }
    }
    
private:
    const Graph graph_;
    const int num_vertices_;
    const int start_vertex_;
    
    std::vector<int> vertex_sequence_;
    std::vector<uint8_t> vertex_flags_;
    
    std::vector<int> edge_to_; //! For each vertex, which vertex preceded it in the sequence. Undefined for v's not in vertex_sequence_!
    std::vector<int> depth_to_;//! For each vertex, what is the depth to the vertex. Undefined for v's not in vertex_sequence_!
};


class BFS
{
public:
    BFS(const Graph &graph, const int start_vertex) :
    graph_(graph), num_vertices_(graph.get_num_vertices()), start_vertex_(start_vertex)
    {
        vertex_sequence_.reserve(num_vertices_);
        vertex_flags_.resize(num_vertices_, 0);
        edge_to_.resize(num_vertices_, 0);
        depth_to_.resize(num_vertices_, 0);
        
        bfs(start_vertex_);
    }
    
    const std::vector<int> &get_vertex_sequence()
    {
        return vertex_sequence_;
    }
    
    const std::vector<int> &get_edge_to()
    {
        return edge_to_;
    }
    
    const std::vector<int> &get_depth_to()
    {
        return depth_to_;
    }
    
    bool has_path_to(const int w) const
    {
        return (vertex_flags_[w] == 1);
    }
    
    std::vector<int> get_path_to(const int w)
    {
        std::vector<int> path;
        int depth = depth_to_[w];
        int v = w;
        path.resize(depth+1, 0);
        
        for (; depth>=0; --depth)
        {
            path[depth] = v;
            v = edge_to_[v];
        }
        
        return path;
    }
    
private:
    void bfs(const int v)
    {
        int queue[num_vertices_];
        
        queue[0] = v;
        vertex_flags_[v] = 1;
        int head=0, tail=1;
        
        while (head!=tail)
        {
            const int head_v=queue[head];
            vertex_sequence_.push_back(head_v);
            
            const std::vector<int> &adj = graph_.get_adjacent_vertices(head_v);
            
            for (int j=0; j<adj.size(); ++j)
            {
                const int w = adj[j];
                if (vertex_flags_[w] == 0)
                {
                    queue[tail] = w;
                    edge_to_[w] = head_v;
                    depth_to_[w] = depth_to_[head_v] + 1;
                    vertex_flags_[w] = 1;
                    tail+=1;
                }
            }
            
            head+=1;
        }
    }
    
private:
    const Graph graph_;
    const int num_vertices_;
    const int start_vertex_;
    
    std::vector<int> vertex_sequence_;
    std::vector<uint8_t> vertex_flags_;
    
    std::vector<int> edge_to_; //! For each vertex, which vertex preceded it in the sequence. Undefined for v's not in vertex_sequence_!
    std::vector<int> depth_to_;//! For each vertex, what is the depth to the vertex. Undefined for v's not in vertex_sequence_!
};


//! Find connected components.
class CC
{
public:
    CC(const Graph &graph) :
    graph_(graph)
    {
        const int num_vertices = graph.get_num_vertices();
        
        vertex_group_ids_.resize(num_vertices, -1);
        int id = 0;
        
        for (int v=0; v<num_vertices; ++v)
        {
            if (vertex_group_ids_[v] == -1)
            {//Vertex not part of a connected component yet.
                //DFS gs(graph, v);
                BFS gs(graph, v);
                
                std::vector<int> component_vertices = gs.get_vertex_sequence();
                
                const int component_size = component_vertices.size();
                for (int i=0; i<component_size; ++i)
                {
                    vertex_group_ids_[component_vertices[i]] = id;
                }
                
                id += 1;
            }
        }
    }
    
    bool is_connected(const int v, const int w) const
    {
        return vertex_group_ids_[v] == vertex_group_ids_[w];
    }
    
    int get_id(const int v) const
    {
        return vertex_group_ids_[v];
    }
    
    const std::vector<int> &get_vertex_group_ids_() const
    {
        return vertex_group_ids_;
    }
    
private:
    const Graph graph_;
    std::vector<int> vertex_group_ids_;
};


// =======================================


Graph g;
//g.add_edge(0, 1);
g.add_edge(0, 2);
//g.add_edge(0, 3);
g.add_edge(1, 4);
g.add_edge(1, 5);
g.add_edge(2, 6);
g.add_edge(2, 7);
g.add_edge(3, 8);
g.add_edge(3, 9);
g.add_edge(4, 5);
//g.add_edge(5, 6);
g.add_edge(6, 7);
g.add_edge(7, 8);
g.add_edge(8, 9);

DFS dfs(g, 2);

std::cerr << "\n";
std::cerr << "dfs.get_vertex_sequence() = " << dfs.get_vertex_sequence() << "\n";
std::cerr << "dfs.get_edge_to() = " << dfs.get_edge_to() << "\n";
std::cerr << "dfs.get_depth_to() = " << dfs.get_depth_to() << "\n";
std::cerr << dfs.has_path_to(9) << "\n";
std::cerr << dfs.get_path_to(9) << "\n";

BFS bfs(g, 2);

std::cerr << "\n";
std::cerr << "bfs.get_vertex_sequence() = " << bfs.get_vertex_sequence() << "\n";
std::cerr << "bfs.get_edge_to() = " << bfs.get_edge_to() << "\n";
std::cerr << "bfs.get_depth_to() = " << bfs.get_depth_to() << "\n";
std::cerr << bfs.has_path_to(9) << "\n";
std::cerr << bfs.get_path_to(9) << "\n";

CC cc(g);

std::cerr << "\n";
std::cerr << "Connected component IDs = " << cc.get_vertex_group_ids_() << "\n";



