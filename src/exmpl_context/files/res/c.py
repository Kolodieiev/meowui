# Змінює усі кольори окрім обраного в заданий.

import re

input_file = "lua.h"
output_file = "lua_data_modified.h"
custom_value = "0x6C9A"
preserve_value = "0xf81f"  # в нижньому регістрі

with open(input_file, "r") as f:
    content = f.read()

pattern = r"(const\s+unsigned\s+short\s+LUA_IMG\[\d+\]\s*=\s*\{)(.*?)(\};)"
match = re.search(pattern, content, re.DOTALL)
if match:
    start, array_body, end = match.groups()

    def replace_hex(m):
        val = m.group(0)
        return val if val.lower() == preserve_value else custom_value

    new_array_body = re.sub(r"0x[0-9a-fA-F]{4}", replace_hex, array_body)
    new_content = start + new_array_body + end
    content = content[:match.start()] + new_content + content[match.end():]

    with open(output_file, "w") as f:
        f.write(content)

    print("Значення замінено")
else:
    print("Масив LUA_IMG не знайдено.")
