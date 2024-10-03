class Grid {

    columns = []
    data = []

    constructor(data){
        this.data = data
        if(this.data.length){
            this.columns = Object.keys(this.data[0]);
        }
        this.columns = ['puzzle'];
    }
    createRow(record){
        const el = document.createElement('div');
        el.classList.add('row');
        this.columns.forEach((column)=>{
            const col = document.createElement('pre');
            col.classList.add('col');
            col.textContent = record[column];
            el.appendChild(col);
        })
        return el;
    }
    render(el){
        this.data.forEach((record)=>{
            const row = this.createRow(record);
            el.appendChild(row);
        })
    }

}


class App {
    container = null
    constructor(){
        this.container = document.body;
    }
    async renderProcessList(){
        const processes = await this.getProcessList();
        const grid = new Grid(processes);
        grid.render(this.container);
    }
    async getProcessList() {
        return fetch('/processes') 
        .then(response => {
          if (!response.ok) {               
            throw new Error('Network response was not ok');
          }
          return response.json();          
        })
        .then(data => {
          console.log(data); 
          return data;                
        })
        .catch(error => {
          console.error('There was a problem with the fetch operation:', error);
        });

    }

}

let app;

document.addEventListener('DOMContentLoaded',(e)=>{
    app = new App();
    setInterval(async ()=>{
        const el = document.createElement('div');
        el.classList.add("grid-container");
        app.container = el;
        await app.renderProcessList();
        window.requestAnimationFrame(()=>{
        document.body.innerHTML = '';
            //let nodes = document.body.childNodes;// .forEach(el=>{
          //  el.remove();
        //})
        document.body.appendChild(app.container);
    });
    },100);
    console.info("KAAR\n");
})