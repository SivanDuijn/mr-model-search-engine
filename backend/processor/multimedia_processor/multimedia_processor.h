// multimedia_processor.h : Include file for standard system include files,
// or project specific include files.

#pragma once

using namespace std;

// TODO move mesh loading to main function?
void debug();
void preprocess(const string database = "../../frontend/model-viewer/public/PSBDatabase", const string in = "69.off", const string out = "69_processed.off");
void preprocess_all(const string database = "../../frontend/model-viewer/public/PSBDatabase");
void extract(const string database = "../../frontend/model-viewer/public/PSBDatabase", const string in = "69_processed.off");
void extract_all(const string database = "../../frontend/model-viewer/public/PSBDatabase");
void compute_feature_vectors(const string database = "../../frontend/model-viewer/public/PSBDatabase");
vector<tuple<int,float>>  query_database_model(const string database = "../../frontend/model-viewer/public/PSBDatabase", const string in = "125_processed.off", size_t k = 10);
void compute_closest_models(const string database = "../../frontend/model-viewer/public/PSBDatabase");
void query_top_k_models(const string database, const string file, int k);
