from cgitb import text
import requests
from bs4 import BeautifulSoup

models_list = []

def brrr(database):
    URL = "https://shape.cs.princeton.edu/benchmark/classifications/v1/" + database + "_database/"

    page = requests.get(URL)
    soup = BeautifulSoup(page.content, "html.parser")

    results = soup.find_all("a", text=lambda t: "categories" not in t )

    categories = list(map(lambda r: r.text, results));
    classes = []

    for i in range(0, len(categories)):
        category = categories[i].split("___")
        if len(category) == 1:
            classes.append([category[0]])
        else:
            classes.append([category[0],category[1]])

    for i in range(0, len(categories)):
        model_url = URL + categories[i] + ".html"
        modelpage = requests.get(model_url)
        modelsoup = BeautifulSoup(modelpage.content, "html.parser")
        modelresults = modelsoup.find_all("tt")
        models = list(map(lambda r: r.text.split(" ")[1], modelresults));

        print(len(categories), i, models)

        for model in models:
            if len(classes[i]) == 2:
                m = [model, classes[i][0], classes[i][1]]
            else: 
                m = [model, None, classes[i][0]]

            models_list.append(m)

brrr("train")
brrr("test")

csv = "Model,Superclass,Class"
for model in models_list:
    csv += "\n"
    for i in range(0, len(model) - 1):
        if model[i] is not None:
            csv += model[i]
        csv += ","
    csv += model[len(model) - 1] 

csv_file = open("../database.csv", "w")
n = csv_file.write(csv)
csv_file.close()
print(n)


