class Grid2 {

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

class Grid {

    constructor(data){
        this.cols = 5
        this.data = data ? data : []
        this.sort = data.sort 
        this.styleElement = null
    }
    get(index){
        return this.data[index] ? this.data[index] : null
    }
    insert(record){
        this.data.push(record)
    }
    renderStyle(container){
        const el = document.createElement('style');
        el.textContent = `
        .rgrid {
        font-size:16px;
            display: 'grid';
            grid-template-columns: repeat(${this.cols}, 1fr);
            grid-template-rows: auto;
            gap: 0px;
            border-radius: 5px;
            clear: both;
            color: #efefef;
            /*aspect-ratio: 1/1;*/
        }
        .rgrid-item {
            width:relative;
            padding: 10px;
            /*aspect-ratio: 1/1*/;
            text-align: left;
            float: left;
            background-color: #000;

        }
        .rgrid->pre {
            width: 100%;
        }    
        `
        container.appendChild(el)
        return el
    }
    render(container){
        if(this.styleElement == null){
            this.styleElement = this.renderStyle(container)
            
        }
        
        const el = document.createElement('div')
        el.classList.add('rgrid');
        this.data.forEach((record)=>{
            const cell = this.createCell(record);
            el.appendChild(cell);
        })
            
            for(let i =0; i< container.children.length; i++){
            if(container.children[i] != this.styleElement){ 
               
                container.children[i].remove()
            };
        }
          
        
                container.appendChild(el)
        
        return el
    }
    createCell(record){
        const el = document.createElement('div');
        el.classList.add('rgrid-item');
        const pre = this.createPre(record);
        el.appendChild(pre);
       // el.textContent = record;
        return el;
    }
    createPre(content){
        const el = document.createElement('pre');
        el.textContent = content;
        return el;
    }

}

const getPuzzleNode = (str, readOnly, size, darkMode) => {
    darkMode = true
    const sudoku = document.createElement('my-sudoku')
    sudoku.setAttribute('puzzle', str ? str : 'generate')
    if(readOnly)
    sudoku.setAttribute('read-only', 'true')
    sudoku.classList.add('sudoku')
    if(darkMode){
        sudoku.classList.add('sudoku-dark')
    }
    return sudoku
}

const renderPuzzleFromString = (str, readOnly, size, darkMode) => {
    darkMode = true
    const sudoku = document.createElement('my-sudoku')
    sudoku.setAttribute('puzzle', str ? str : 'generate')
    if(readOnly)
    sudoku.setAttribute('read-only', 'true')
    sudoku.classList.add('sudoku')
    if(darkMode){
        sudoku.classList.add('sudoku-dark')
    }
    document.body.appendChild(sudoku);
    const id = generateIdByPosition(sudoku);
    sudoku.id = id
    return sudoku
}

class App {
    container = null
    constructor(){
    }
    async renderProcessList(container){

        const processes = await this.getProcessList();
       // processes.forEach(process=>{
       //     renderPuzzleFromString(process.puzzle,false,13,true)
       // })

       const grid = new Grid(processes.map(process=>{
        return [process.puzzle ,
             "duration: " +process.duration.toString(),
             "initial: "+process.result_initial_count.toString(),
             "steps: ","  "+process.steps.toString(),
             "steps total: ","  "+process.steps_total.toString(),
             "solved:" + process.solved_count.toString()
            ].join('\n')
        }));
       grid.render(container);
    }

    async renderStatistics(container){
        const statistics = await this.statistics;
        const column_one = `longest_running: ${statistics.longest_running}
solved_total: ${statistics.solved_total}
start: ${statistics.start}
steps_per_puzzle: ${statistics.steps_per_puzzle}
steps_total: ${statistics.steps_total}
time_winner: ${statistics.time_winner}`
        const column_two = statistics.puzzle_winner

        const column_three = statistics.solution_winner

        const data = [column_one,column_two,column_three]
        const grid = new Grid(data)
        grid.render(container)
      
    }
     get statistics() {
        return fetch('/json/statistics/').then(response=>{
            return response.json();
        }).then(data=>{
            console.debug(data)
            return data
        })
    }
    async getProcessList() {
        return fetch('/json/processes') 
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

const asyncAnimationFrame = async(callback)=>{
    return new Promise((resolve)=>{
        requestAnimationFrame(()=>{
            resolve(callback());
        })
    })
}


let app;

document.addEventListener('DOMContentLoaded',(e)=>{
    app = new App();
    const refreshProcesList = async() =>{
       setInterval(async()=>{
            await asyncAnimationFrame(async ()=>{
                const processes = await app.getProcessList();
                
                newNodes = processes.map(process=>{
                   return getPuzzleNode(process.puzzle,false,13,true)
                })
                let hasChildren = document.body.childNodes[0].length ? true : false 
                let count = 0;
                newNodes.forEach(node=>{
                    document.body.appendChild(node)
                    count++;
              
             
            })
            if(hasChildren)
                for(let i = 0; i < count; i++){
                      document.body.childNodes[0].remove()
                      
                } 
        })
        },1000);
    }

    //refreshProcesList();
    setInterval(async()=>{
        const el = document.getElementById('statistics');
        asyncAnimationFrame(async ()=>{
        
        await app.renderStatistics(el);
        });
    },200);
    setInterval(async ()=>{
        const el =  document.getElementById('details')
        
        asyncAnimationFrame(async ()=>{
            await app.renderProcessList(el);
        })
        
    },200);
    console.info("KAAR\n");
})