


function randInt(min, max) {
    min = Math.ceil(min);
    max = Math.floor(max);
    return Math.floor(Math.random() * (max - min + 1)) + min;
}

class EventHandler {
    constructor() {
        // An object to store events and their corresponding listeners
        this.events = {};
    }

    // Method to add an event listener
    on(event, listener) {
        // If the event doesn't exist, create an array for it
        if (!this.events[event]) {
            this.events[event] = [];
        }

        // Add the listener to the event's array
        this.events[event].push(listener);
    }

    // Method to remove an event listener
    off(event, listenerToRemove) {
        if (!this.events[event]) return;

        // Filter out the listener to remove
        this.events[event] = this.events[event].filter(listener => listener !== listenerToRemove);
    }

    // Method to emit an event and call all listeners
    emit(event, data) {
        if (!this.events[event]) return;

        // Call each listener for the event with the data
        this.events[event].forEach(listener => listener(data));
    }
}


class Col extends EventHandler {
    id = 0
    values = []
    initial = false
    row = null
    index = 0
    valid = true
    _selected = false
    _value = 0
    _isValidXy() {
        if (!this._value)
            return true;
        return this.row.puzzle.fields.filter(field => {
            return field != this && !(
                field.index == this.index && field.value == this._value
                ||
                field.row.index == this.row.index && field.value == this._value)

        })
    }
    select() {
        this.selected = true
    }
    unselect() {
        this.selected = false
    }
    _isValidBox() {
        if (!this._value)
            return true;
        let startRow = this.row.index - this.index % (this.row.puzzle.size / 3);
        let startCol = this.index - this.index % (this.row.puzzle.size / 3);
        for (let i = 0; i < this.row.puzzle.size / 3; i++) {
            for (let j = 0; j < this.row.puzzle.size / 3; j++) {
                const field = this.row.puzzle.get(i + startRow, j + startCol);
                if (field != this && field.value == this._value) {
                    return false;
                }
            }
        }
        return true
    }
    validate() {
        if (!this.row.puzzle.initialized) {
            return true
        }
        if (this.initial) {

            this.valid = true
            return this.valid
        }
        if (!this.value) {
            if (this.valid) {
                this.emit('update', this)
            }
            this.valid = true
            return this.valid
        }
        let oldValue = this.valid
        this.valid = this._isValidXy() && this._isValidBox();
        if (oldValue != this.valid) {
            this.emit('update', this)
        }

        return this.valid
    }
    set value(val) {
        if (this.initial)
            return;
        if (this._value != Number(val)) {
            this._value = Number(val)
            this.validate()
            this.emit('update', this);
        }
    }
    get value() {
        return this._value
    }
    get selected() {
        return this._selected
    }
    set selected(val) {
        if (val != this._selected) {
            this._selected = val
            this.emit('update', this);
        }
    }
    constructor(row) {
        super()
        this.row = row
        this.index = this.row.cols.length
        this.id = this.row.puzzle.rows.length * this.row.puzzle.size + this.index;
        this._initial = false
        this._value = 0;
        this.valid = true
    }
    update() {
        this.validate()
    }
    toggleSelected() {
        if (this.selected)
            this.unselect()
        else
            this.select()
    }
    get data() {
        return {
            values: this.values,
            value: this.value,
            index: this.index,
            id: this.id,
            row: this.row.index,
            col: this.index,
            valid: this.valid,
            initial: this.initial,
            selected: this.selected
        }
    }
    toString() {
        return String(this.value)
    }
    toText() {
        return this.toString().replace("0", " ");
    }
}
class Row extends EventHandler {
    cols = []
    puzzle = null
    index = 0
    initialized = false
    constructor(puzzle) {
        super()
        this.puzzle = puzzle
        this.cols = []
        this.index = this.puzzle.rows.length
        const me = this
        this.initialized = false
        for (let i = 0; i < puzzle.size; i++) {
            const col = new Col(this);
            this.cols.push(col);
            col.on('update', (field) => {
                me.emit('update', field)
            })
        }
        this.initialized = true
    }
    get data() {
        return {
            cols: this.cols.map(col => col.data),
            index: this.index
        }
    }
    toText() {
        let result = ''
        for (let col of this.cols) {
            result += col.toText();
        }
        return result
    }
    toString() {
        return this.toText().replaceAll(" ", "0");
    }
}

class Puzzle extends EventHandler {
    rows = []
    size = 0
    hash = 0
    states = []
    parsing = false
    _initialized = false
    initialized = false
    _fields = null
    constructor(arg) {
        super()
        this.initalized = false
        this.rows = []
        if (isNaN(arg)) {
            // load session
        } else {
            this.size = Number(arg)
        }
        for (let i = 0; i < this.size; i++) {
            const row = new Row(this);
            this.rows.push(row);
            row.on('update', (field) => {
                this.onFieldUpdate(field)
            })
        }
        this._initialized = true
        this.initalized = true
    }
    reset() {
        this._initialized = false
        this.parsing = true
        this.fields.forEach(field => {
            field.value = 0
            field.initial = false
            field.selected = false
        })
        this.hash = 0
        this.states = []
        this.parsing = false
        this._initialized = true
    }
    get valid() {
        return this.invalid.length == 0
    }
    get invalid() {
        return this.fields.filter(field => !field.valid)
    }
    get selected() {
        return this.fields.filter(field => field.selected)
    }
    loadString(content) {
        this.emit('parsing', this)
        this.reset()
        this.parsing = true
        this._initialized = false;
        const regex = /\d/g;
        const matches = [...content.matchAll(regex)]
        let index = 0;
        const max = this.size * this.size;
        matches.forEach(match => {
            const digit = Number(match[0]);
            let field = this.fields[index]
            field.value = digit;
            field.initial = digit != 0
            field.update()
            index++;
        });
        this._initialized = true;
        this.parsing = false
        this.commitState()
        this.emit('parsed', this)
    }
    get state() {
        return this.getData(true)
    }
    get previousState() {
        if (this.states.length == 0)
            return null;
        return this.states.at(this.states.length - 1)
    }
    get stateChanged() {
        if (!this._initialized)
            return false
        return !this.previousState || this.state != this.previousState
    }

    commitState() {
        this.hash = this._generateHash()
        if (this.stateChanged) {
            this.states.push(this.state)
            this.emit('commitState', this)
            return true
        }
        return false
    }
    onFieldUpdate(field) {
        if (!this._initialized)
            return;
        this.commitState()
    }

    get data() {
        return this.getData(true)
    }

    popState() {
        let prevState = this.previousState
        if (!prevState)
            return null
        while (prevState && prevState.hash == this.state.hash)
            prevState = this.states.pop()
        if (!prevState)
            return null
        this.applyState(prevState)
        this.emit('popState', this)
        return prevState
    }
    applyState(newState) {

        this._initialized = false
        newState.fields.forEach(stateField => {
            let field = this.get(stateField.row, stateField.col)
            field.selected = stateField.selected
            field.values = stateField.values
            field.value = stateField.value
            field.initial = stateField.initial
            field.validate()
        })
        this._initialized = true
        this.emit('stateApplied', this)
        this.emit('update', this)
    }
    getData(withHash = false) {
        let result = {
            fields: this.fields.map(field => field.data),
            size: this.size,
            valid: this.valid
        }
        if (withHash) {
            result['hash'] = this._generateHash()
        }
        return result;
    }
    get(row, col) {
        return this.rows[row].cols[col];
    }
    get fields() {
        if (this._fields == null) {
            this._fields = []
            for (let row of this.rows) {
                for (let col of row.cols) {
                    this._fields.push(col)
                }
            }
        }
        return this._fields
    }
    _generateHash() {
        var result = 0;
        JSON.stringify(this.getData(false)).split('').map(char => {
            return char.charCodeAt(0) - '0'.charCodeAt(0)
        }).forEach(num => {
            result += 26
            result = result + num
        })
        return result
    }
    get text() {
        let result = ''
        for (let row of this.rows) {
            result += row.toText() + "\n"
        }
        result = result.slice(0, result.length - 1)
        return result
    }
    get initialFields() {
        return this.fields.filter(field => field.initial)
    }
    get json() {
        return JSON.stringify(this.data)
    }
    get zeroedText() {
        return this.text.replaceAll(" ", "0")
    }
    get string() {
        return this.toString()
    }
    toString() {
        return this.text.replaceAll("\n", "").replaceAll(" ", "0")
    }
    getRandomField() {
        const emptyFields = this.empty;
        return emptyFields[randInt(0, emptyFields.length - 1)]
    }
    update(callback) {
        this._initialized = false
        callback(this);
        this._initialized = true
        this.commitState()
        this.emit('update', this)
    }
    get empty() {
        return this.fields.filter(field => field.value == 0)
    }
    getRandomEmptyField() {
        let field = this.getRandomField()
        if (!field)
            return null
        return field
    }
    deselect() {
        this.fields.forEach(field => field.selected = false)
    }
    generate(){
        this.reset()
        this.initialized = false
        for(let i =0; i < 17; i++){
            this.fillRandomEmptyField()
        }
        this.states = []
        this.commitState()
    }
    fillRandomEmptyField() {
        let field = this.getRandomEmptyField()
        if (!field)
            return
        this.deselect()
        field.selected = true
        let number = 0
        number++;
        
        while (number <= 9) {
            field.value = number 
            field.update()
            if(field.validate()){
                field.initial = true
                return field 
            }
            number++;
        }
        return false;
    }

}

let activePuzzle = null
function puzzleToElement(puzzle, size, darkMode) {

    const main = document.createElement('div');
    //main.style.minWidth = '50px'
    main.style.aspectRatio = '1/1';
    main.classList.add('grid-container')
    //main.style.aspectRatio = '1/1'
    let fields = [];
    let rows = [];
    let syncEvents = [];
    let fieldElements = [];
    puzzle.rows.forEach((row) => {
        rows.push(row)
        row.cols.forEach((col) => {

            const field = document.createElement('div');
            field.style.fontSize = size.toString() + 'px'
            field.classList.add('grid-item');
            fields.push(col)
            fieldElements.push(field);
            //// field.setData('col',col);
            // field.setData('row',row);
            //field.setData('puzzle',puzzle);
            // field.style.padding = '20px';
            //  field.style.width = '50px'
            //  field.style.border = '1px solid #ccc';
            // field.style.textAlign = 'center'
            // field.style.height = '10%'
            //field.style.alignItems = 'center';
            //field.style.alignContent = 'center';
            //field.style.textAlign = 'center';
            //field.style.aspectRatio = '1/1';
            field.field = col
            field.sync = function sync() {
                field.classList.remove('sudoku-field-selected')
                field.classList.remove('sudoku-field-empty')
                field.classList.remove('sudoku-field-invalid')
                field.textContent = field.field.value ? field.field.value.toString() : ''

                if (field.field.selected) {
                    field.classList.add('sudoku-field-selected')
                    window.selected = field.field
                }
                if (!field.field.valid) {
                    field.classList.add('sudoku-field-invalid')
                }
                if(!field.field.value)
                {

                field.classList.add('sudoku-field-empty')
                }
            }
            col.on('update', (e) => {
                field.sync();
            })
            syncEvents.push(field.sync)
            field.sync()
            field.addEventListener('click', (e) => {
                col.toggleSelected()
            })

            //  row.appendChild(field)
            main.appendChild(field)
            main.toggleDarkMode =  (e)=>{
                main.classList.toggle('sudoku-theme-dark')
            }
            main.addEventListener('mouseenter',(e)=>{
                activePuzzle = puzzle
            })
        });
    });
    if(darkMode){
        main.toggleDarkMode()
    }
    if(activePuzzle == null){
    document.addEventListener('keydown', (e) => {
        if (e.key == 'u') {
            activePuzzle.popState();
        } else if (e.key == 'd') {
            activePuzzle.update((target) => {
                activePuzzle.selected.forEach(field => {
                    field.value = 0
                });
            })
        } else if (e.key == 'a'){
            function autoSolve() {
            window.requestAnimationFrame(()=>{
                if(activePuzzle.fillRandomEmptyField()){
                    if(activePuzzle.empty.length)
                        return autoSolve()
                    else
                        return true 
                    return true 
                }
            })
        }
            autoSolve()
        } else if (e.key == 'r') {
            activePuzzle.fillRandomEmptyField();
        } else if (!isNaN(e.key)) {
            activePuzzle.update((target) => {
                activePuzzle.selected.forEach(field => {
                    field.value = Number(e.key)
                })
            });
        }
        //if(e.key == 'u'){
        //    puzzle.popState()
        //}
    })
    }

    main.fieldElements = fieldElements;
    main.fields = fields
    main.rows = rows
    let syncFn = () => {
        syncEvents.forEach(fn => fn())
    }
    main.syncEvents = syncFn

    //  puzzle.on('update',(e)=>{
    //    console.info('update');
    //    main.syncEvents()
    /// })
    puzzle.on('update', (e) => {
        console.info('updateEvent');
        main.syncEvents()
    })
    main.sync = syncFn;
    activePuzzle = puzzle
    return main
    //main.appendChild(row)
}
let puzzles = []
function generateIdByPosition(element) {
    // Get the parent element
    const parent = element.parentNode;
    
    // Get the index of the element within its parent
    const index = Array.prototype.indexOf.call(parent.children, element);
    
    // Generate a unique ID using the tag name and index
    const generatedId = `${element.tagName.toLowerCase()}-${index}`;
    
    // Assign the generated ID to the element
    element.id = generatedId.replace('div-','session-key-');
    
    return element.id ;
}


const renderPuzzleFromString = (str,size,darkMode) => {
    const app = new Puzzle(9)
    if(str)
        app.loadString(str)
    else
        app.generate()
    const appElement = puzzleToElement(app,size,darkMode);
    
    document.body.appendChild(appElement);
    const id = generateIdByPosition(appElement);
    appElement.id = id
    console.info(id);
    puzzles.push(appElement)
    return appElement
}

const renderPuzzles = (size, darkMode)=>{
    renderPuzzleFromString(null,size,darkMode)
    
}

document.addEventListener('DOMContentLoaded', (e) => {
    let size = randInt(4,20);
    let darkMode = randInt(0,1);
    renderPuzzleFromString(`0  0  8  0  9  0  0  0  0 
 0  9  0  0  0  0  0  0  7 
 0  0  0  0  2  0  0  0  0 
 0  0  3  0  0  0  0  0  0 
 5  0  0  0  8  0  0  0  0 
 0  0  6  0  0  0  0  0  0 
 0  0  0  0  0  3  9  0  0 
 9  1  0  0  0  0  0  0  0 
 0  0  0  0  1  0  0  0  0 `,size,darkMode)
    renderPuzzleFromString(`2  3  8  1  9  7  4  5  6 
        4  9  1  3  5  0  2  8  7 
        6  5  0  0  0  0  1  3  9 
        1  2  3  0  4  9  0  6  8 
        5  7  9  6  8  1  3  2  4 
        8  4  0  7  3  2  0  9  1 
        7  8  4  2  6  3  9  1  5 
        9  1  2  8  7  5  0  4  3 
        3  6  5  9  1  4  8  7  0
       `,size,darkMode)
       renderPuzzleFromString(` 0  0  8  0  9  0  0  0  0 
        0  9  0  0  0  0  0  0  7 
        0  0  0  0  2  0  0  0  0 
        0  0  3  0  0  0  0  0  0 
        5  0  0  0  8  0  0  0  0 
        0  0  6  0  0  0  0  0  0 
        0  0  0  0  0  3  9  0  0 
        9  1  0  0  0  0  0  0  0 
        0  0  0  0  1  0  0  0  0`,size, darkMode)
    for(let i = 0; i < 10; i++){
    renderPuzzles(size,darkMode)
   }
});