# Processor

### Building

Make sure to have cmake and make setup.
Choose a compiler of your choice. 

For reference, the one we used for Mac was `Apple clang version 14.0.0` targeting `arm64-apple-darwin21.6.0`.


```bash
# cd into this folder
# configure cmake
cmake -S . -B out

# build
cmake --build out
```

After building, the executable is copied to the server (`backend/server/processors`), this way the server can easily call the processor.

### Running

```bash
# The executable can be called with the following arguments
multimedia_processor [COMMAND] [DATABASE_DIR] [IN_MODEL] [OUT_MODEL]

[COMMAND] can be one of the following:
 - preprocess               remeshes and normalizes a given model
 - preprocess-all           preprocesses all the models in the database
 - extract                  calculates features for a given model
 - extract-all              extracts features for all models in the database
 - compute-dist-matrix      precomputes distances between all models in the database
 - compute-ann              computes the approximate nearest neighbor index
 - query-database-model     quick query of a model that is already in the database
 - query-database-model-ann quick query of a model in datababase using ANN
 - query-model              queries a model outside the database
 - compute-closest-models   precomputes top 10 closest models for each model in the database

[DATABASE_DIR] specifies the database directory, 
default: ../../frontend/model-viewer/public/PSBDatabase

[IN_MODEL]  (optional) input model file name to process
[OUT_MODEL] (optional) output filename

# Examples
# Run the whole preprocessing pipeline by calling the executable with these parameters
./out/multimedia_processor/multimedia_processor preprocess-all

./out/multimedia_processor/multimedia_processor extract-all

./out/multimedia_processor/multimedia_processor compute-all-closest


# Specify the location of the database

./out/multimedia_processor/multimedia_processor ../../frontend/model-viewer/public/PSBDatabase preprocess-all
```
