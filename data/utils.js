function fetchAndDisplayIP() {
    return new Promise((resolve, reject) => {
      axios
        .get("/ip")
        .then(function (response) {
          const ip = response.data;
          document.getElementById("ipEsp").value = ip;
          console.log("ESP IP Address: " + ip);
          resolve();
        })
        .catch(function (error) {
          console.error("Error fetching IP:", error);
          reject(error);
        });
    });
  }