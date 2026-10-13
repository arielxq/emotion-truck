from datetime import datetime

from backend.app.services.state_machine import StateMachine


def test_arrival_and_direction_selection():
    machine = StateMachine()
    now = datetime(2026, 10, 10, 14, 0)
    machine.arrive()
    machine.joystick("UP", False, now)
    result = machine.status(now)
    assert result["state"] == "ARRIVED"
    assert result["selected_emotion"] == "ㄍㄢˋ"
    assert result["can_throw"] is True


def test_returned_can_be_confirmed_back_to_idle():
    machine = StateMachine()
    now = datetime(2026, 10, 10, 14, 0)
    machine.arrive()
    machine.joystick("LEFT", False, now)
    machine.joystick("LEFT", True, now)
    assert machine.data.state in {"COLLECTED", "RETURNED"}
    if machine.data.state == "RETURNED":
        machine.joystick(None, True, now)
        assert machine.data.state == "IDLE"
