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

### Running

```bash
# Run the whole preprocessing pipeline by calling the executable with these parameters

./out/multimedia_processor/multimedia_processor preprocess-all

./out/multimedia_processor/multimedia_processor extract-all

./out/multimedia_processor/multimedia_processor compute-all-closest


# It's also possible to specify the location of the database

./out/multimedia_processor/multimedia_processor ../../frontend/model-viewer/public/PSBDatabase preprocess-all
```
