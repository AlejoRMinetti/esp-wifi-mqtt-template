console.log("ESP32 template script");

// Set up a global variable to store the state of the LED
var ledOn = false;

// Define the toggleLED function
function toggleLED() {
  // Toggle the state of the LED
  ledOn = !ledOn;

  // Send a request to the ESP32 to turn the LED on or off
  // Replace http://192.168.1.100 with the IP address of your ESP32
  fetch(`http://192.168.1.100/?led=${ledOn}`)
    .then(response => {
      // Log the response from the ESP32
      console.log(response);
    })
    .catch(error => {
      // Log any errors that occur
      console.error(error);
    });

  // TODO: cambiar la imagen unicamente si se recibe respuesta OK
  // o imagen de error
  if(ledOn){
    document.getElementById("card-led-state").className = "card-led card-led-on";
    document.getElementById("LED-state-img").src="./images/LED on.png"
  }else{
    document.getElementById("card-led-state").className = "card-led card-led-off";
    document.getElementById("LED-state-img").src="./images/LED off.png"
  }
}