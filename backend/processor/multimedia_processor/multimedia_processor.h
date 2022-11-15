// multimedia_processor.h : Include file for standard system include files,
// or project specific include files.

#pragma once

using namespace std;

// TODO move mesh loading to main function?
void debug();
void preprocess(const string in = "69.off", const string out = "69_processed.off");
void preprocess_all();
void extract(const string in = "69_processed.off");
void extract_all();
vector<tuple<int,float>>  query_database_model(const string in = "125_processed.off", const size_t k = 10);
void compute_closest_models();
void query_top_k_models(const string file, int k);
void evaluate(const size_t k = 10);
