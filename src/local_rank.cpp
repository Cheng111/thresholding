#include "local_rank.h"

int local_rank(igraph_t& G,
			   int d, // select d-largest ranking vertices
			   igraph_t& new_G){

    igraph_integer_t E = igraph_ecount(&G);
    igraph_integer_t V = igraph_vcount(&G);

	igraph_vit_t v_iterator;
	igraph_vit_create(&G, igraph_vss_all(), &v_iterator);
	igraph_integer_t v_id;

    // place true in vector to choose edges to keep
    igraph_vector_t bool_keep_edges;
    igraph_vector_init(&bool_keep_edges, E);


	while (!IGRAPH_VIT_END(v_iterator)) {
		v_id = IGRAPH_VIT_GET(v_iterator);

		igraph_es_t incident_edges; //selector
	  	igraph_es_incident(&incident_edges, v_id, IGRAPH_ALL);

		igraph_integer_t num_neighbours;
	  	igraph_es_size(&G, &incident_edges, &num_neighbours);

		igraph_eit_t e_iterator;
	  	igraph_eit_create(&G, incident_edges, &e_iterator);

		igraph_integer_t e_id;

	  	if (num_neighbours <= d){
	  		//we're going to add all of these edges anyway
		  	while (!IGRAPH_EIT_END(e_iterator)){
	  			e_id = IGRAPH_EIT_GET(e_iterator);
		  		VECTOR(bool_keep_edges)[e_id] = 1;
		  		IGRAPH_EIT_NEXT(e_iterator);
		  	}
  		}
  		else{
		  	// get all weights of incident edges
			igraph_real_t w;

  		  	std::vector<double> incident_weights(num_neighbours);
			igraph_vector_t incident_eids;
			igraph_vector_init(&incident_eids, num_neighbours);

		  	int i=0; // neighbour index for incident_weights
		  	while (!IGRAPH_EIT_END(e_iterator)){
		  		e_id = IGRAPH_EIT_GET(e_iterator);
		  		w = igraph_cattribute_EAN(&G, "weight", e_id);

		  		incident_weights[i] = w;
		  		VECTOR(incident_eids)[i] = e_id;

		  		IGRAPH_EIT_NEXT(e_iterator);
		  		i++;
		  	}

	  		// get <d> largest edge weights (absolute-value)
  			std::vector<size_t> d_indices = argsort(incident_weights, d);
  			for (i=0; i<d_indices.size(); i++){
  				VECTOR(bool_keep_edges)[d_indices[i]] = 1;
  			}
  		}
	  IGRAPH_VIT_NEXT(v_iterator);
	}
	igraph_vit_destroy(&v_iterator);

	// collect e_ids
	// TODO use igraph_vector_view??
    igraph_vector_t edge_indices;
    igraph_vector_init(&edge_indices, 0);
    for (int i; i<E; i++){
    	if (VECTOR(bool_keep_edges)[i] ==1){
			igraph_vector_push_back(&edge_indices, i);
    	}
    }

  	// induce graph with these edges, delete non-adjacent vertices=True
  	igraph_subgraph_edges(&G, &new_G, igraph_ess_vector(&edge_indices), 1);

	igraph_vector_destroy(&bool_keep_edges);
	igraph_vector_destroy(&edge_indices);

	return 0;
}
