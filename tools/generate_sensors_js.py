Import("env")
from pathlib import Path


def generate_js(sensors):
    lines = ['// Auto-generated file', '', 'export const availableSensors = [']
    for s in sensors:
        lines.append(f'  "{s}",')
    lines.append('];')
    return '\n'.join(lines)

def generate_js_file(source, target, env):
    print("Writing static/sensors.js file")
    sensors = ['camera', 'temp', 'humidity']
    js_code = generate_js(sensors)

    static_path = Path("static")
    static_path.mkdir(exist_ok=True)
    with open(static_path / "sensors.js", "w") as f:
        f.write(js_code)

env.AddPreAction("$BUILD_DIR/${PROGNAME}.elf", generate_js_file)