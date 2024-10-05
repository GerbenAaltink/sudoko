


function randInt(min, max) {
    min = Math.ceil(min);
    max = Math.floor(max);
    return Math.floor(Math.random() * (max - min + 1)) + min;
}

function cssRuleExists(match){
    for(let i = 0; i < document.styleSheets.length; i++){
        let styleSheet = document.styleSheets[i]
        let rules = styleSheet.cssRules
        for(let j = 0; j < rules.length; j++){
            let rule = rules[j]
            if(rule.selectorText && rule.selectorText == match){
                return true;
            }
        }
    }
    return false
}

class EventHandler {
    constructor() {
        this.events = {};
        this.eventCount = 0;
        this.suppresEvents = false;
        this.debugEvents = false;
    }
    on(event, listener) {
        if (!this.events[event]) {
            this.events[event] = { name:name, listeners: [],callCount: 0};
        }
        this.events[event].listeners.push(listener);
    }
    off(event, listenerToRemove) {
        if (!this.events[event]) return;
        this.events[event].listeners = this.events[event].listeners.filter(listener => listener !== listenerToRemove);
    }
    emit(event, data) {
        if (!this.events[event]) return [];
        if (this.suppresEvents) return [];
        this.eventCount++;
        const returnValue = this.events[event].listeners.map(listener =>{
            var returnValue = listener(data)
            if(returnValue == undefined)
                return null
            return returnValue
        });
        this.events[event].callCount++;
        if(this.debugEvents){
            console.debug('debugEvent',{event:event, arg:data, callCount: this.events[event].callCount, number:this.eventCount, returnValues:returnValue})
        }
        return returnValue
    }
    suppres(fn) {
        const originallySuppressed = this.suppresEvents
        this.suppresEvents = true 
        fn(this)
        this.suppresEvents = originallySuppressed
        return originallySuppressed
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
            return (
                field.index == this.index && field.value == this._value
                ||
                field.row.index == this.row.index && field.value == this._value)

        }).filter(field => field != this).length == 0
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
        let startRow = this.row.index - this.row.index % (this.row.puzzle.size / 3);
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
        if (!this.row.puzzle.initalized) {
            return this.valid;
        }
        if (this.initial) {
            this.valid = true
            return this.valid
        }
        if (!this.value && !this.valid) {
            this.valid = true
            this.emit('update', this)
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
        const digit = Number(val)
        const validDigit = digit >= 0 && digit <= 9;
        let update = validDigit && digit != this.value
        if (update) {
            this._value = Number(digit)
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
            if(this.row.puzzle.initalized)
            this.emit('update', this);
        }
    }
    constructor(row) {
        super()
        this.row = row
        this.index = this.row.cols.length
        this.id = this.row.puzzle.rows.length * this.row.puzzle.size + this.index;
        this.initial = false
        this.selected = false
        this._value = 0;
        this.valid = true
    }
    update() {
        this.emit('update',this)
    }
    toggleSelected() {
        this.selected = !this.selected 
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
    initalized = false
    _fields = null
    constructor(arg) {
        super()
        this.debugEvents = true;
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
        this.commitState()
    }
    validate() {
        return this.valid;
    }
    _onEventHandler(){
        this.eventCount++;
    }
    makeInvalid() {
        if (!app.valid) {
            let invalid = this.invalid;
            return invalid[invalid.length - 1];
        }
        this.rows.forEach(row => {
            row.cols.forEach(col => {
                if (col.value) {
                    let modify = null;
                    if (col.index == this.size) {
                        modify = this.get(row.index, col.index - 2);
                    } else {
                        modify = this.get(row.index, col.index + 1);
                    }
                    modify.value = col.value
                    // last one is invalid
                    return modify.index > col.index ? modify : col;
                }
                col.valid = false
            })
        })
        this.get(0, 0).value = 1;
        this.get(0, 1).value = 1;
        return this.get(0, 1);
    }
    reset() {
        this._initialized = false
        this.initalized == false;
        this.parsing = true
        this.fields.forEach(field => {
            field.initial = false
            field.selected = false
            field.value = 0
        })
        this.hash = 0
        this.states = []
        this.parsing = false
        this.initalized = true
        this._initialized = true
        this.commitState()
    }
    get valid() {
        return this.invalid.length == 0
    }
    get invalid() {
        this.emit('validating',this)
        const result =  this.fields.filter(field => !field.validate())
        this.emit('validated',this)
        return result
    }
    get selected() {
        return this.fields.filter(field => field.selected)
    }
    loadString(content) {
        this.emit('parsing', this)
        this.reset()
        this.parsing = true
        this.initalized = false;
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
            index++;
        });
        this._initialized = true;
        this.parsing = false
        this.deselect();
        this.initalized = true;
        this.suppres(()=>{
            this.fields.forEach((field)=>{
                field.update()
        })
    })
        this.commitState()
        this.emit('parsed', this)
        this.emit('update',this)
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
        if (!this.initalized)
            return false;
        this.hash = this._generateHash()
        if (this.stateChanged) {
            this.states.push(this.state)
            this.emit('commitState', this)
            return true
        }
        return false
    }
    onFieldUpdate(field) {
        if (!this.initalized)
            return false;
        if (!this._initialized)
            return;
        this.validate();
        this.commitState();
        this.emit('update', this)
    }

    get data() {
        return this.getData(true)
    }

    popState() {
        let prevState = this.previousState
        if (!prevState)
        {
            this.deselect()   
            return null
        }while (prevState && prevState.hash == this.state.hash)
            prevState = this.states.pop()
        if (!prevState)
        {
            this.deselect()
            return null
        }
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
        if (!this.initalized)
            return null;
        if (!this.rows.length)
            return null;
        if (!this.rows[row])
            return null;
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
        return this.text.replaceAll("\n","").replaceAll(" ", "0")
    }
    get humanFormat() {
        return ' ' + this.text.replaceAll(" ", "0").split("").join(" ")
    }
    getRandomField() {
        const emptyFields = this.empty;
        return emptyFields[randInt(0, emptyFields.length - 1)]
    }
    update(callback) {
        this.commitState()
        this.intalized = false
        callback(this);
        this.intalized = true
        this.validate()
        this.commitState()
        this.intalized = false 
        if(this.valid)
            this.deselect()
        this.intalized = true
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
    generate() {
        this.reset()
        this.initalized = false
        for (let i = 0; i < 17; i++) {
            this.fillRandomField()
        }
        this.deselect()
        this.initalized = true
        this.commitState()
        this.emit('update',this)
    }
    fillRandomField() {
        let field = this.getRandomEmptyField()
        if (!field)
            return
        this.deselect()
        field.selected = true
        let number = 0
        number++;

        while (number <= 9) {
            field.value = randInt(1, 9)
            field.update()
            if (this.validate()) {
                field.initial = true
                return field
            }
            number++;
        }
        return false;
    }

}

class PuzzleManager {
    constructor(size) {
        this.size = size
        this.puzzles = []
        this._activePuzzle = null
    }

    addPuzzle(puzzle){
        this.puzzles.push(puzzle)
    }
    get active(){
        return this.activePuzzle
    }
    set activePuzzle(puzzle){
        this._activePuzzle = puzzle
    }
    get activePuzzle(){
        return this._activePuzzle
    }
}

const puzzleManager = new PuzzleManager(9)

class Sudoku extends HTMLElement {
    styleSheet = `
        .sudoku {
            font-size: 13px;
            color:#222;
            display: grid;
            grid-template-columns: repeat(9, 1fr);
            grid-template-rows: auto;
            gap: 0px;
            user-select: none;
            -webkit-user-select: none;
            -moz-user-select: none;
            -ms-user-select: none;
            background-color: #e5e5e5;
            border-radius: 5px;
            aspect-ratio: 1/1;
        }
        .sudoku-field-initial {
            color: #777;
        }
        .sudoku-field-selected {
            background-color: lightgreen;
        }
        .sudoku-field-invalid {
            color: red;
        }
        .sudoku-field {
            border: 1px solid #ccc;
            text-align: center;
            padding: 2px;
            aspect-ratio: 1/1;
        }`
    set fieldSize(val) {
        this._fieldSize = val ? Number(val) : null
        this.fieldElements.forEach(field => {
            field.style.fontSize = this._fieldSize ? this._fieldSize.toString() +'px' : ''
        })
    }
    get fieldSize(){
        return this._fieldSize
    }
    get eventCount() {
        return this.puzzle.eventCount
    }
    get puzzleContent(){
        return this.puzzle.humanFormat
    }
    set puzzleContent(val) {
        if (val == "generate") {
            this.puzzle.generate()
        } else if (val) {
            this.puzzle.loadString(val)
        } else {
            this.puzzle.reset()
        }
    }
    connectedCallback() {
        this.puzzleContent = this.getAttribute('puzzle') ? this.getAttribute('puzzle') : null
        this._fieldSize = null
        this.fieldSize = this.getAttribute('size') ? this.getAttribute('size') : null
        this.readOnly = this.getAttribute('read-only') ? true : false
        this.attachShadow({ mode: 'open' });
            this.shadowRoot.appendChild(this.styleElement)
        this.shadowRoot.appendChild(this.puzzleDiv)
    }
    toString(){
        return this.puzzleContent
    }
    set active(val){
        this._active = val 
        if(this._active)
            this.manager.activePuzzle =this 
    }
    get active(){
        return this._active
    }
    set readOnly(val){
        this._readOnly = val ? true : false
    }
    get readOnly(){
        return this._readOnly
    }
    constructor() {
        super();
        this._readOnly = false;
        this._active = false
        this.fieldElements = []
        this.puzzle = new Puzzle(9)
        this.fields = []
        this.styleElement = document.createElement('style');
        this.styleElement.textContent = this.styleSheet
        this.puzzleDiv = document.createElement('div')
        this.puzzleDiv.classList.add('sudoku');
        this._bind()
        this.manager.addPuzzle(this)
    }
    get manager() {
        return puzzleManager
    }
    _bind(){
        this._bindFields()
        this._bindEvents()
        this._sync()
    }
    _bindFields(){
        const me = this
        this.puzzle.rows.forEach((row) => {
            row.cols.forEach((field) => {
                const fieldElement = document.createElement('div');
                fieldElement.classList.add('sudoku-field');
                fieldElement.field = field
                field.on('update', (field) => {
                    me._sync()
                })
                fieldElement.addEventListener('click', (e) => {
                    if(!me.readOnly)
                        field.toggleSelected()
                })
                fieldElement.addEventListener('contextmenu',(e)=>{
                    e.preventDefault()
                        field.row.puzzle.update(()=>{
                            field.selected = false
                            field.value = 0
                    })
                })
                this.fields.push(field)
                this.fieldElements.push(fieldElement)
                this.puzzleDiv.appendChild(fieldElement);
            });
        });
    }
    _bindEvents(){
        const me = this
        this.puzzle.on('update', () => {
            me._sync()
        });
        this.puzzleDiv.addEventListener('mouseenter', (e) => {
            me.active = true 
        })
        this.puzzleDiv.addEventListener('mouseexit', (e) => {
            me.active = false 
        })
        document.addEventListener('keydown', (e) => {
            if(me.readOnly)
                return
            if (!puzzleManager.active)
                return
            const puzzle = puzzleManager.active.puzzle
            if (e.key == 'u') {
                puzzle.popState();
            } else if (e.key == 'd') {
                puzzle.update((target) => {
                    puzzle.selected.forEach(field => {
                        field.value = 0
                    });
                })
            } else if (e.key == 'a') {
                puzzle.autoSolve()
            } else if (e.key == 'r') {
                puzzle.fillRandomField();
            } else if (!isNaN(e.key)) {
                puzzle.update((target) => {
                    puzzle.selected.forEach(field => {
                        field.value = Number(e.key)
                    })
                });
            }
        })
    }
    autoSolve() {
        const me = this
        window.requestAnimationFrame(() => {
            if (me.fillRandomField()) {
                if (me.empty.length)
                    return me.autoSolve()
            }
        })
    }
    get(row, col){
        return this.puzzle.get(row,col)
    }
    _syncField(fieldElement) {
        const field = fieldElement.field 
        fieldElement.classList.remove('sudoku-field-selected')
        fieldElement.classList.remove('sudoku-field-empty')
        fieldElement.classList.remove('sudoku-field-invalid')
        fieldElement.classList.remove('sudoku-field-initial')
        fieldElement.innerHTML = field.value ? field.value.toString() : '&nbsp;'
        
        if (field.selected) {
            fieldElement.classList.add('sudoku-field-selected')
            window.selected = field.field
        }
        if (!field.valid) {
            fieldElement.classList.add('sudoku-field-invalid')
        }
        if (!field.value) {
            fieldElement.classList.add('sudoku-field-empty')
        }
        if(field.initial){
            fieldElement.classList.add('sudoku-field-initial')
        }

    }
    _sync() {
        this.fieldElements.forEach(fieldElement => {
            this._syncField(fieldElement);
        })
    }

}
customElements.define("my-sudoku", Sudoku);

function generateIdByPosition(element) {
    // Get the parent element
    const parent = element.parentNode;

    // Get the index of the element within its parent
    const index = Array.prototype.indexOf.call(parent.children, element);

    // Generate a unique ID using the tag name and index
    const generatedId = `${element.tagName.toLowerCase()}-${index}`;

    // Assign the generated ID to the element
    element.id = generatedId.replace('div-', 'session-key-');

    return element.id;
}

