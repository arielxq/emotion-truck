from fastapi.testclient import TestClient

from backend.app.main import app, machine


client = TestClient(app)


def setup_function():
    machine.reset()


def test_status_endpoint():
    response = client.get("/api/status")
    assert response.status_code == 200
    assert response.json()["state"] == "IDLE"


def test_demo_flow_can_reach_arrived():
    response = client.post("/api/arrive")
    assert response.status_code == 200
    assert response.json()["state"] == "ARRIVED"
