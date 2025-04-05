def generate_js(sensors):
    lines = ['// Auto-generated file', '', 'export const availableSensors = [']
    for s in sensors:
        lines.append(f'  "{s}",')
    lines.append('];')
    return '\n'.join(lines)