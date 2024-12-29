const url = "http://192.168.185.3";

const btnPump = document.querySelector("#btnPump");
const btnFan = document.querySelector("#btnFan");


console.log(btnPump);
console.log(btnFan);


function getRelayPompa() {
  fetch(`${url}/kontrolPompa`)
    .then((res) => res.text())
    .then((hasil) => {
      if (hasil == "NYALA") {
        btnPump.style.backgroundColor = "red";
        btnPump.textContent = "MATIKAN";
        imgPump.src = "img/icons8-pump-100 (1).png";
      } else {
        btnPump.style.backgroundColor = "green";
        btnPump.textContent = "NYALAKAN";
        imgPump.src = "img/icons8-pump-100.png";
      }
    })
    .catch((err) => {
      console.log("pump tidak bisa diakses", err);
    });
}

function setRelayPompa() {
  fetch(`${url}/kontrolPompa`, {
    method: "POST",
  })
    .then((res) => res.text())
    .then(() => getRelayPompa())
    .catch((err) => {
        console.log("Pump tidak bisa di control", err);
    })
}

function getRelayKipas() {
    fetch(`${url}/kontrolFan`)
      .then((res) => res.text())
      .then((hasil) => {
        if (hasil == "NYALA") {
          btnFan.style.backgroundColor = "red";
          btnFan.textContent = "MATIKAN";
          imgFan.src = "img/fan2.png"
        } else {
          btnFan.style.backgroundColor = "green";
          btnFan.textContent = "NYALAKAN";
          imgFan.src = "img/fan1.png"
        }
      })
      .catch((err) => {
        console.log("fan tidak bisa diakses", err);
      });
  }
  
  function setRelayKipas() {
    fetch(`${url}/kontrolFan`, {
      method: "POST",
    })
      .then((res) => res.text())
      .then(() => getRelayKipas())
      .catch((err) => {
          console.log("Fan tidak bisa di control", err);
      })
  }


document.addEventListener("DOMContentLoaded", () => {
    getRelayPompa();
    getRelayKipas();
});