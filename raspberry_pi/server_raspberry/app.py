from flask import Flask, request, redirect, url_for, render_template
from pymongo import MongoClient

cliente = MongoClient("localhost", 27017)
banco = cliente["monitoramento"]
colecao = banco["objetos"]

app = Flask(__name__)

if __name__ == '__main__':
    app.run()


@app.route('/')
def inicio():
    cursor = colecao.find({})
    result = list(cursor)

    results = [{'id': '1', 'situacao': 'normal'}, {'id': '2', 'situacao': 'fora da rota'}, {'id': '2', 'situacao': 'normal'}]
    return render_template("home.html", results=results)


@app.route("/cadastrar")
def cadastrar_objeto():
    return render_template("calc_route.html")


@app.route("/monitorar")
def monitorar_objeto():
    return render_template("monitorar.html")


@app.route("/salvar_coord/<int:id>", methods=['POST'])
def salvar_coordenadas(id):
    busca = {"id": id}
    resultado = list(colecao.find(busca))
    if len(resultado) != 0:
        print("Objeto ja existe")
        return redireciona_erro()
    # recebe json da rota via javascript
    rota = request.get_json(force=True)['route']
    documento_objeto = {"id": id, "coordenadas": rota}
    colecao.insert_one(documento_objeto)

    print("Objeto incluído")
    return render_template('home.html')

def redireciona_erro():
    return render_template("calc_route.html", message='Objeto já existe')




