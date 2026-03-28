import os
import time
import socket
import subprocess
import csv
from flask import Flask, render_template, request, send_file

app = Flask(__name__)

HOST = '127.0.0.1'
PORT = 8080
OUTPUT_FILE = 'network_rep.csv'
ENGINE_EXEC = './flower_exchange'

@app.route('/')
def index():
    return render_template('index.html', show_results=False)

@app.route('/process', methods=['POST'])
def process_file():
    if 'file' not in request.files:
        return "No file uploaded", 400
    
    file = request.files['file']
    if file.filename == '':
        return "No file selected", 400

    # 1. Read uploaded CSV into memory
    lines = file.read().decode('utf-8').splitlines()
    if len(lines) > 0 and "Client Order ID" in lines[0]:
        lines.pop(0)

    # 2. Start the C++ Matching Engine
    print("Starting C++ Engine...")
    engine_process = subprocess.Popen([ENGINE_EXEC])
    time.sleep(0.5)

    # 3. Establish TCP Connection & Blast Data
    start_time = time.time()
    try:
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
            s.connect((HOST, PORT))
            for line in lines:
                s.sendall(f"{line}\n".encode('utf-8'))
            s.sendall(b"EOF,Rose,1,10,10.0\n")
    except ConnectionRefusedError:
        engine_process.kill()
        return "Error: Could not connect to the C++ Engine on port 8080.", 500

    # 4. Wait for Engine to finish
    engine_process.wait()
    exec_time_ms = (time.time() - start_time) * 1000

    # 5. Parse the generated CSV for the UI Preview
    if not os.path.exists(OUTPUT_FILE):
        return "Error: Engine did not generate output file.", 500

    headers = []
    preview_rows = []
    total_rows = 0

    with open(OUTPUT_FILE, 'r') as f:
        reader = csv.reader(f)
        try:
            headers = next(reader)
        except StopIteration:
            pass
        
        for row in reader:
            total_rows += 1
            if total_rows <= 100:  # Only send the first 100 rows to the browser
                preview_rows.append(row)

    # Render the index page again, but this time switch to the Results View
    return render_template('index.html', 
                           show_results=True, 
                           headers=headers, 
                           rows=preview_rows, 
                           total_rows=total_rows,
                           exec_time=f"{exec_time_ms:.2f}")

# 6. New Route specifically for downloading the full file
@app.route('/download')
def download_file():
    if os.path.exists(OUTPUT_FILE):
        return send_file(OUTPUT_FILE, as_attachment=True, download_name='execution_rep.csv')
    return "File not found", 404

if __name__ == '__main__':
    if not os.path.exists(ENGINE_EXEC):
        print(f"ERROR: {ENGINE_EXEC} not found. Please run ./build.sh first.")
        exit(1)
    print("Starting Web Portal on http://127.0.0.1:5000")
    app.run(debug=True, port=5000)