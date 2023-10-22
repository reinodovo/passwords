def contents(path):
    with open(path, 'r') as file:
        return file.read()

template = contents("./manual/template.html")
table = contents("./manual/table.html")
manual = template.replace("{{table}}", table)
with open("./manual/pages/page1.html", 'w') as file:
    file.write(manual)