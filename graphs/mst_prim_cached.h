struct Vertex
{
    Vertex(const int x, const int y) : x_(x), y_(y) {}
    int x_, y_;
};

struct Edge
{
    Edge(const int i, const int j) : i_(i), j_(j) {}
    int i_, j_;
};

int N_ = 1000;
int W_ = 800;
int H_ = 600;

std::vector<Vertex> vertices_;
std::vector<int> vertex_flags_;
std::vector<int> vertex_closest_index_;
std::vector<int> vertex_closest_dist_sq_;

std::vector<Edge> edges_;

void generate_vertices()
{
    edges_.clear();
    
    vertices_.clear();
    vertices_.reserve(N_);
    
    for (int i=0; i<N_; ++i)
    {
        vertices_.emplace_back(rng_.next(W_), rng_.next(H_));
    }
}

#ifdef TCSDL
void render_graph(const uint8_t r, const uint8_t g, const uint8_t b)
{
    sdl::clear_buffer();
    
    for (int vi=0; vi<N_; ++vi)
    {
        sdl::render_dot(vertices_[vi].x_, vertices_[vi].y_,
                        0, 0, 0);
    }
    
    for (int ei=0; ei<edges_.size(); ++ei)
    {
        sdl::render_line(vertices_[edges_[ei].i_].x_, vertices_[edges_[ei].i_].y_,
                         vertices_[edges_[ei].j_].x_, vertices_[edges_[ei].j_].y_,
                         r, g, b);
    }
    
    sdl::update_display();
}
#endif

void build_mst_prim_naive()
{//O(m+nlog(n)) number of edges m and number of vertices n.
    vertex_flags_.clear();
    vertex_flags_.resize(N_, 0);
    
    edges_.clear();
    edges_.reserve(N_);
    
    const int start_vi = 0;
    vertex_flags_[start_vi] = 1;
    
    for (;;)
    {
        int best_vi = -1;
        int best_wi = -1;
        int best_vw_sq = std::numeric_limits<int>::max();
        
        for (int vi=0; vi<N_; ++vi)
        {
            if (vertex_flags_[vi]!=0)
            {//vertices_[vi] part of MST.
                const Vertex &V = vertices_[vi];
                
                int vertex_closest_index = -1;
                int vertex_closest_dist_sq = std::numeric_limits<int>::max();
                
                //Find vertex closest to vi.
                for (int wi=0; wi<N_; ++wi)
                {
                    if (vertex_flags_[wi]==0)
                    {//vertices_[wi] not yet part of MST.
                        const Vertex &W = vertices_[wi];
                        
                        const int dx = V.x_ - W.x_;
                        const int dy = V.y_ - W.y_;
                        const int vw_sq = dx*dx + dy*dy;
                        
                        if (vw_sq < vertex_closest_dist_sq)
                        {//wi is closer...
                            vertex_closest_index = wi;
                            vertex_closest_dist_sq = vw_sq;
                        }
                    }
                }
                
                if (vertex_closest_dist_sq < best_vw_sq)
                {//Potential MST edge VW is shorter ...
                    best_wi = vertex_closest_index;
                    best_vi = vi;
                    best_vw_sq = vertex_closest_dist_sq;
                }
            }
        }
        
        if (best_vi < 0) break;
        
        edges_.emplace_back(best_vi, best_wi);
        vertex_flags_[best_wi] = 1;
    }
}

void build_mst_prim_cached()
{
    vertex_flags_.clear();
    vertex_flags_.resize(N_, 0);
    
    //Cache of closest vertex to MST vertices...
    vertex_closest_index_.clear();
    vertex_closest_index_.resize(N_, -1);
    vertex_closest_dist_sq_.clear();
    vertex_closest_dist_sq_.resize(N_, std::numeric_limits<int>::max());
    
    edges_.clear();
    edges_.reserve(N_);
    
    const int start_vi = 0;
    vertex_flags_[start_vi] = 1;
    
    for (;;)
    {
        int best_vi = -1;
        int best_wi = -1;
        int best_vw_sq = std::numeric_limits<int>::max();
        
        for (int vi=0; vi<N_; ++vi)
        {
            if (vertex_flags_[vi] != 0)
            {//vertices_[vi] part of MST.
                const Vertex &V = vertices_[vi];
                
                if (vertex_closest_index_[vi] < 0)
                {//Calc closest vertex if not yet cached...
                    int vertex_closest_index;
                    int vertex_closest_dist_sq = vertex_closest_dist_sq_[vi];
                    
                    //Find vertex closest to vi.
                    for (int wi=0; wi<N_; ++wi)
                    {
                        if (vertex_flags_[wi]==0)
                        {//vertices_[wi] not yet part of MST.
                            const Vertex &W = vertices_[wi];
                            
                            const int dx = V.x_ - W.x_;
                            const int dy = V.y_ - W.y_;
                            const int vw_sq = dx*dx + dy*dy;
                            
                            if (vw_sq < vertex_closest_dist_sq)
                            {//wi is closer...
                                vertex_closest_index = wi;
                                vertex_closest_dist_sq = vw_sq;
                            }
                        }
                    }
                    
                    //Optimisation - Hit the array just once ...
                    vertex_closest_index_[vi] = vertex_closest_index;
                    vertex_closest_dist_sq_[vi] = vertex_closest_dist_sq;
                }
                
                if (vertex_closest_dist_sq_[vi] < best_vw_sq)
                {//Potential MST edge VW is shorter ...
                    best_wi = vertex_closest_index_[vi];
                    best_vi = vi;
                    best_vw_sq = vertex_closest_dist_sq_[vi];
                }
            }
        }
        
        if (best_vi < 0) break;
        
        edges_.emplace_back(best_vi, best_wi);
        vertex_flags_[best_wi] = 1;
        
        //Invalidate all vertices of MST that had as closest wi.
        for (int i=0; i<N_; ++i)
        {
            if ((vertex_flags_[i] == 1) &&
                (vertex_closest_index_[i] == best_wi))
            {
                vertex_closest_index_[i] = -1;
                vertex_closest_dist_sq_[i] = std::numeric_limits<int>::max();
            }
        }
    }
}

//======================================
//======================================
//======================================

generate_vertices();

for (;;) {
    
    const double start_time_prim_naive = TCTimer::get_tsc_time();
    build_mst_prim_naive();
    const double end_time_prim_naive = TCTimer::get_tsc_time();
    
#ifdef TCSDL
    render_graph(255, 0, 0);
#endif
    
    const double start_time_prim_cached = TCTimer::get_tsc_time();
    build_mst_prim_cached();
    const double end_time_prim_cached = TCTimer::get_tsc_time();
    
#ifdef TCSDL
    render_graph(0, 255, 0);
#endif
    
    DBN(end_time_prim_naive - start_time_prim_naive)
    DBN(end_time_prim_cached - start_time_prim_cached)



