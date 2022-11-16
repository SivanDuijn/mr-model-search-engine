<h1 align="center">
    Monorepo for the<br>Utrecht University Multimedia Retrieval assignment
</h1>

## Monorepo split up the following:

    .
    ├── backend
    │   ├── processor     # c++ CMake project with all the core logic
    │   └── server        # A Python API endpoint to query models
    │
    ├── frontend          # A NextJS web app as a graphical user interface
    └── utils       

Find more info about how to build and run all these in their respective folders.

### Processor
The processor creates an executable which is used for the MR pipeline. It contains functionality to preprocess the database, extract features and query a model, in or outside the database.

### Server
The server is an API build with Python using Flask. It creates an endpoint (`/query/api`) which accepts a `.off` or `.obj` model and responds with a json containing the processed model, descriptors and top 10 closest models with their distances. The server does this by calling the executable created by the processor with the right parameters. 

### Frontend
The frontend is a website build with NextJS and provides a UI to view the models, inspect the descriptors and show the top 10 closest models. If the API is also running, it is also possible to upload models outside the database and query those.

This is also where our database resides `frontend/model-viewer/public/PSBDatabase/`
