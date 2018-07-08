from flask import Flask, request, render_template

app = Flask(__name__)

if __name__ == '__main__':
    app.run()

@app.route('/')
def hello_world():
    return 'Hello World!'

@app.route("/cadastrar")
def cadastrar_objeto():
	return render_template("calc_route.html")

@app.route("/monitorar")
def monitorar_objeto():
	return render_template("monitorar.html")

@app.route("/salvar_coord/<int:id>", methods = ['POST'])
def salvar_coordenadas(id):
	#recebe json da rota via javascript
	arrayDaRota = request.get_json(force=True)['route']
