from flask import Flask, request, render_template

app = Flask(__name__)

@app.route("/inicio")
def funcao_da_pagina_inicio():
	return "Funcionou!"

@app.route("/cadastrar")
def cadastrar_objeto():
	return render_template("calc_route.html")

@app.route("/monitorar")
def monitorar_objeto():
	return render_template("monitorar.html")

@app.route("/salvar_coord/<int:id>", methods = ['POST'])
def salvar_coordenadas(id):
	#recebe json da rota via javascript
	jsonDaRota = request.get_json(force=True)





app.run(port=5000, debug=True)