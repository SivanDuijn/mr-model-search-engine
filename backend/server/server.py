from flask import Flask
import multimedia_processor

app = Flask(__name__)

@app.route("/")
def index():
    return "<h1>You're not supposed to be here!</h1> <p>Go make some API calls or something \"¬¬</p>"

@app.route("/debug")
def debug():
    result = multimedia_processor.call(["debug"])
    return f"<h1>Check the console for a message from the backend processor!{result}</h1>"