from flask import Flask, jsonify, request
from flightsrun import run
import json
import os
from flask_cors import CORS

app = Flask(__name__)
# Enable CORS for our React frontend; allow preflight and content-type header
CORS(
    app,
    resources={r"/api/*": {"origins": ["http://localhost:5174"]}},
    supports_credentials=False,
    allow_headers=["Content-Type"],
    methods=["GET", "POST", "OPTIONS"]
)

def load_data_from_file():
    try:
        if os.path.exists('output.json'):
            with open('output.json', 'r') as file:
                return json.load(file)
        else:
            return {"message": "No data available."}
    except Exception as e:
        return {"error": f"Could not load data from file: {e}"}

@app.route('/api/data', methods=['GET', 'POST', 'OPTIONS'])
def handle_data():
    # Flask-CORS will handle OPTIONS and add required headers
    if request.method == 'POST':
        data = request.get_json(force=True)
        source = data.get('source')
        destination = data.get('destination')
        day = data.get('day')
        departure_time = data.get('departure_time')

        if not all([source, destination, day, departure_time]):
            return jsonify({"error": "Missing one or more required fields"}), 400

        # Run the flight-finder logic
        print(f"Request: {source} → {destination} on {day} at {departure_time}")
        run(source, destination, day, departure_time)

        # Load the generated output.json
        output_data = load_data_from_file()
        return jsonify(output_data)

    # GET or other
    return jsonify({"message": "Use POST with JSON body to get flight data."})

if __name__ == "__main__":
    app.run(host="0.0.0.0", port=5000)
