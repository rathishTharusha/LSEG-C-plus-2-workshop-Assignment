# 1. Use the official Ubuntu runtime
FROM ubuntu:22.04

# 2. Prevent interactive prompts from blocking the build
ENV DEBIAN_FRONTEND=noninteractive

# 3. Install C++ compiler, Boost networking libraries, and Flask in one step
RUN apt-get update && apt-get install -y \
    g++ \
    libboost-all-dev \
    python3 \
    python3-flask \
    && rm -rf /var/lib/apt/lists/*

# 4. Set the working directory inside the container
WORKDIR /app

# 5. Copy your project files into the container (respects .dockerignore)
COPY . /app

# 6. Compile the C++ matching engine natively inside the Linux container
RUN sed -i 's/\r$//' build.sh && chmod +x build.sh && bash build.sh

# 7. Expose the port your Flask app is running on
EXPOSE 5000

# 8. Boot up the Python web server
CMD ["python3", "app.py"]