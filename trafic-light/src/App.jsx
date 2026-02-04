import tailwindcss from "tailwindcss";
import React, { useState, useEffect } from "react";

export default function App() {
  const [red, setRed] = useState("gray");
  const [yellow, setYellow] = useState("gray");
  const [green, setGreen] = useState("gray");

  const ESP32_IP = "192.168.5.5";

  const controlLED = async (led, state) => {
    try {
      await fetch(`http://${ESP32_IP}/led?led=${led}&state=${state}`);
    } catch (error) {
      console.error("Error controlling LED:", error);
    }
  };

  const toggleGreen = () => {
    setRed("gray");
    setYellow("gray");
    setGreen("green");
    controlLED("red", "off");
    controlLED("yellow", "off");
    controlLED("green", "on");
  };

  const toggleYellow = () => {
    setRed("gray");
    setYellow("yellow");
    setGreen("gray");
    controlLED("red", "off");
    controlLED("yellow", "on");
    controlLED("green", "off");
  };

  const toggleRed = () => {
    setRed("red");
    setYellow("gray");
    setGreen("gray");
    controlLED("red", "on");
    controlLED("yellow", "off");
    controlLED("green", "off");
  };

  const traficLightSequence = () => {
    // Green light - Go
    toggleGreen();
    setTimeout(() => {
      // Yellow light - Prepare to stop
      toggleYellow();
      setTimeout(() => {
        // Red light - Stop
        toggleRed();
        setTimeout(() => {
          // Red + Yellow together - Prepare to go (common in some countries)
          setRed("red");
          setYellow("yellow");
          setGreen("gray");
          controlLED("red", "on");
          controlLED("yellow", "on");
          controlLED("green", "off");
          setTimeout(() => {
            // Back to Green
            toggleGreen();
          }, 2000);
        }, 10000);
      }, 2000);
    }, 10000);
  };


  const stopSequence = () => {
    setRed("gray");
    setYellow("gray");
    setGreen("gray");
    controlLED("red", "off");
    controlLED("yellow", "off");
    controlLED("green", "off");
  };

  return (
    <div>
      <div>
  
        <button onClick={toggleGreen} style={{ backgroundColor: green }}>
          Green
        </button>
        <button onClick={toggleYellow} style={{ backgroundColor: yellow }}>
          Yellow
        </button>
        <button onClick={toggleRed} style={{ backgroundColor: red }}>
          Red
        </button>  
        </div>
<div>
  
        <button onClick={traficLightSequence}>Start Sequence</button>
        <button onClick={stopSequence}>Stop Sequence</button>
        
</div>
    </div>
  );
}
