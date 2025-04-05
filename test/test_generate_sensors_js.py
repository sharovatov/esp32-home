import unittest
from tools.sensors_js_generator import generate_js

class TestSensorJsGeneration(unittest.TestCase):
    def test_generates_expected_output(self):
        sensors = ['camera', 'temp', 'humidity']
        result = generate_js(sensors)
        expected = '''// Auto-generated file

export const availableSensors = [
  "camera",
  "temp",
  "humidity",
];'''
        self.assertEqual(result.strip(), expected.strip())