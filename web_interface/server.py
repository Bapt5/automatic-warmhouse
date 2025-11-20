import json
import datetime

from flask import Flask, request, render_template

app = Flask(__name__)

@app.route("/", methods=["GET"])
def index():
    """Root endpoint that serves a simple HTML page."""
    return render_template('dashboard.html')

@app.route("/receive_data", methods=["POST"])
def receive_data():
    """Endpoint to receive data via POST request.
    Expects JSON data in the request body. It saves the received data to an array in 'data.json'.

    Example request
    {
        "air_temperature": 25.5,
        "air_humidity": 60,
        "soil_temperature": 22.3,
        "water_level": 20,
        "light_intensity": 300
    }
    """
    data = request.get_json()

    if not data:
        return "No JSON data received", 400

    # Add a timestamp to the data
    data['timestamp'] = datetime.datetime.now().isoformat()

    # Save the data to a JSON file
    with open("data.json", "r", encoding="utf-8") as json_file:
        try:
            existing_data = json.load(json_file)
        except json.JSONDecodeError:
            print("JSON decode error - initializing new data list.")
            existing_data = []

    existing_data.append(data)
    
    with open("data.json", "w", encoding="utf-8") as json_file:
        json.dump(existing_data, json_file, indent=4)

    return {"status": "success", "data_received": data}, 200

@app.route("/reset_data", methods=["POST"])
def reset_data():
    """Endpoint to reset the data stored in 'data.json'."""
    with open("data.json", "w", encoding="utf-8") as json_file:
        json.dump([], json_file)

    return {"status": "success", "message": "Data has been reset."}, 200

@app.route("/get_data", methods=["GET"])
def get_data():
    """Endpoint to retrieve all stored data from 'data.json'."""
    try:
        with open("data.json", "r", encoding="utf-8") as json_file:
            data = json.load(json_file)
    except (FileNotFoundError, json.JSONDecodeError):
        data = []

    return {"status": "success", "data": data}, 200


if __name__ == "__main__":
    app.run(host="localhost", port=5000)
