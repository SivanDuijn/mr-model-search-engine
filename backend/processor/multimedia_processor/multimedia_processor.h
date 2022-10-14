// multimedia_processor.h : Include file for standard system include files,
// or project specific include files.

#pragma once

using namespace std;

// TODO move mesh loading to main function?
void debug();
void preprocess(const string database = "PSBDatabase", const string in = "69.off", const string out = "69_processed.off");
void preprocessAll(const string database = "PSBDatabase");
void extract(const string database = "PSBDatabase", const string in = "69_processed.off");
