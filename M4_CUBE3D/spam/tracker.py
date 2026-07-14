modules = {
    "Parsing": {
        "Pseudo code": 100,
        "Code": 100,
        "Tests": 90,
        "Debug": 100,
        "Norminette": 100
    },
    "Autres": {
        "Pseudo code": 100,
        "Code": 100,
        "Tests": 90,
        "Debug": 100,
        "Norminette": 0
    }
}


def avg(module):
    if not module:
        return 0
    return int(sum(module.values()) / len(module))


def bar(p):
    filled = int(p / 10)
    return "█" * filled + "░" * (10 - filled)


def global_avg(modules):
    values = []
    for m in modules.values():
        values.append(avg(m))
    return int(sum(values) / len(values))


def generate_md(modules):
    total = global_avg(modules)

    output = f"# CUB3D — {total}% {bar(total)}\n\n---\n\n"

    for name, tasks in modules.items():
        percent = avg(tasks)
        output += f"## {name.upper()} — {percent}% {bar(percent)}\n\n"

        if tasks:
            for task, value in tasks.items():
                output += f"- {task} : {value}%\n"
        else:
            output += "- Pseudo code : 0%\n"
            output += "- Code : 0%\n"
            output += "- Tests : 0%\n"
            output += "- Debug : 0%\n"
            output += "- Norminette : 0%\n"

        output += "\n---\n\n"

    return output


with open("CUB3D_progression.md", "w", encoding="utf-8") as f:
    f.write(generate_md(modules))

print("✅ Fichier généré : CUB3D_progression.md")
