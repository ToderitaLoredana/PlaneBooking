from flask import Flask, jsonify
from flightsrun import run
import json
import os

app = Flask(__name__)

# Function to load data from JSON file
def load_data_from_file():
    try:
        # Check if file exists
        if os.path.exists('output.json'):
            with open('output.json', 'r') as file:
                return json.load(file)
        else:
            print("Warning: output.json file not found. Using default data.")
            return {"message": "No data available. Place 'output.json' file in the same directory as this script."}
    except Exception as e:
        print(f"Error loading JSON file: {e}")
        return {"error": "Could not load data from file"}

# Define API endpoint to return data from JSON file
@app.route('/api/data', methods=['GET'])
def get_data():
    run()
    data = load_data_from_file()
    return jsonify(data)


if __name__ == "__main__":
    app.run(host="0.0.0.0", port=8000)