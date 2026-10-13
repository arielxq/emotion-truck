const $ = (id) => document.getElementById(id);
let selectedDirection = null;

async function request(path, options = {}) {
  const response = await fetch(path, { headers: { "Content-Type": "application/json" }, ...options });
  return response.json();
}

function render(data) {
  $("state").textContent = data.state;
  $("message").textContent = data.message;
  $("clock").textContent = new Date(data.current_time).toLocaleString("zh-TW");
  $("last-collect").textContent = data.last_collect ? new Date(data.last_collect).toLocaleString("zh-TW") : "尚未收運";
  $("selection").textContent = data.selected_emotion ? `已選擇：${data.selected_emotion}` : "尚未選擇情緒";
  $("prompt").textContent = data.worker_prompt || "";
  $("led").className = `led ${data.led.color.toLowerCase()}`;
}

async function refresh() { render(await request("/api/status")); }

document.querySelectorAll("[data-direction]").forEach((button) => button.addEventListener("click", async () => {
  selectedDirection = button.dataset.direction;
  render(await request("/api/joystick", { method: "POST", body: JSON.stringify({ direction: selectedDirection, pressed: false }) }));
}));

$("confirm").addEventListener("click", async () => {
  const data = await request("/api/joystick", { method: "POST", body: JSON.stringify({ direction: selectedDirection, pressed: true }) });
  render(data);
  if (data.state === "COLLECTED") { $("bag").classList.remove("throw"); void $("bag").offsetWidth; $("bag").classList.add("throw"); }
});
$("arrive").addEventListener("click", async () => render(await request("/api/arrive", { method: "POST" })));
$("reset").addEventListener("click", async () => { selectedDirection = null; render(await request("/api/reset", { method: "POST" })); });
refresh();
setInterval(refresh, 1000);
