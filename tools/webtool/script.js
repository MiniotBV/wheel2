

let port;
let reader;
let inputDone;
let outputDone;
let inputStream;
let outputStream;

var sendFormaat="acii";
var sendNewLine = true;
var sendCariageReturn = false;

var serialBuffer = "";
var lineBuffer = "";
var autoScroll = true;
var verbergPlotterData = true;
var ontvagAlleenBijNewLine = true;
var showTimeStamp = true;


var versturen = document.getElementById("txtSend");
var ontvangen = document.getElementById("txtOntvangen");
var tabel = document.getElementById("tblLijnen");
var geselecteerdeSample = document.getElementById("geselecteerdeSample");

var sampleIndex = 0;
var oudSampleIndex = 0;
var plotterDataPunten = []; // dataPoints
var plotterData = [];
var plotterKleuren = ["red", "blue","green", "#edc240", "#afd8f8", "#cb4b4b", "#4da74d", "#9440ed"]//['Tomato', 'blue', 'Lime', 'gold', 'DarkTurquoise', 'magenta', , ]
var plotterTabel = [];
var plotterTitles = [];
var lijnToggle = false;
var serialInterval = Date.now();
var plotterSampleNummer = 1000; // number of dataPoints visible at any point
var plotterTijdInterval = 5;
var plotterPauze = false;
var plotterFormaat = "samples"
var plot = $.plot("#plotter", plotterData, {
    xaxes: [
        { position: 'bottom' }
    ],
    yaxes: [{ 
        position: 'left', 
        autoScale: 'loose', //autoScale: 'off'
        min: -2.0, 
        max: 2.0, 
        autoScaleMargin: 0.1,
        growOnly: false}
    ],
    grid: {
        hoverable: true,
        clickable: true,
        autoHighlight: true,
        borderWidth: 1,  //omringende rand
        color: "black", //kleur van de rand
        markingsColor: "green" //kleur van het grid
    },
    series: {
        lines: {
            show: true,
            lineWidth: 1
        }
    }    
});










$(function() {
    if ('serial' in navigator) {
        $("#notSupported").hide()
    }

    $("#nbrBaudRate")[0].value = 115200;
    $("#nbrPlotterSampleNummer")[0].value = plotterSampleNummer;
    $("#nbrPlotterTijdInterval")[0].value = plotterTijdInterval;


    
    $("<div id='tooltip'></div>").css({
        position: "absolute",
        display: "none",
        // border: "1px solid #fdd",
        padding: "2px",
        backgroundColor: "#eee8",
        // opacity: 1
    }).appendTo("body");





    $("#plotter").bind("plothover", function (event, pos, item) {
        if (item) {

                // $("#tooltip").html(item.series.label + " of " + x + " = " + y)
            $("#tooltip").html(plotterTitles[item.seriesIndex] + " ( " + item.datapoint[0] + " ) = " + item.datapoint[1])
                // .css({top: item.pageY+5, left: item.pageX+5})//, backgroundColor: item.series.color})
                .css({top: pos.pageY+5, left: pos.pageX+5})
                .show(0);
        } else {
            $("#tooltip").hide();
        }
    }).bind("plotclick", function (event, pos, item) {
        document.getElementById("btnPauze").click()
    });



    $("#plotter").bind("plothovercleanup", function (event, pos, item) {
        $("#tooltip").hide();
    });






    var placeholder = $("#placeholder");
        // var plot = $.plot(placeholder, [d1, d2, d3]);

    // placeholder.resize(function () {
    //     $(".message").text("Placeholder is now "
    //         + $(this).width() + "x" + $(this).height()
    //         + " pixels");
    // });

    $(".plotter-container").resizable({
        maxWidth: 2000,
        maxHeight: 1000,
        minWidth: 200,
        minHeight: 100});




    window.requestAnimationFrame(updatePlotter);
});




















function updatePlotter() {
    try {
        for(var i = 0; i < plotterDataPunten.length; i++){
            if(plotterDataPunten[i] && plotterDataPunten[i][0]){
                while(plotterDataPunten[i][0][0] < sampleIndex - plotterSampleNummer){
                    plotterDataPunten[i].shift()
                }
            }
        }
    } catch (error) {
        console.log(error)
    }
    

    

    if(sampleIndex != oudSampleIndex && !plotterPauze || lijnToggle){
        oudSampleIndex = sampleIndex;
        lijnToggle = false
        
        plot.setData(plotterData);
        plot.setupGrid(true);
        plot.draw();
    }


    $('#btnConnect')[0].textContent = port ? "Verbreek" : "Verbind"
    // if(plotterTabel[0]){
    //     $('#groteAmplitude')[0].textContent = plotterTabel[2][3].innerText;
    // }
    


    window.requestAnimationFrame(updatePlotter);
}













async function serialRead() {
    try{
        while (true) {
            const { value, done } = await reader.read();
            // if(value.length > 250){
            //     console.log("hejo: " + value.length);
            //     value = null;
            // }
            if (value) {
                lineBuffer += value;//---------------------------------------------Plotter data
                
                while(lineBuffer.indexOf('\n') != -1){
                    var lines = lineBuffer.split('\n');
                    var line = lines.shift();
                    lineBuffer = lines.join('\n');
                    var nummers = line.split(',')

                    if(line.indexOf("CLR") != -1){
                        $('#txtOntvangen')[0].textContent=  ''
                        serialBuffer = ''
                    }

                    if( !isNaN(parseFloat(nummers[0])) ){ //---------------------------------zijn er nummers in de buffer?

                        //console.log( (serialInterval - Date.now())) 
                        //serialInterval = Date.now()
                        if(!plotterPauze){
                            for(var i = 0;    i < (nummers.length > 20 ? 20 : nummers.length);    i++){//----------------------max 16 sporen

                                if(plotterDataPunten[i] == undefined){
                                    nieuweGrafiek(i)
                                }

                                if( !isNaN( parseFloat(nummers[i]) ) ){
                                    plotterDataPunten[i].push([  sampleIndex,  parseFloat(nummers[i])  ]);
                                    
                                    lijnInfo(plotterDataPunten[i], plotterTabel[i]);
                                }
                            }
                            sampleIndex++;
                        }
                        if(verbergPlotterData){
                            serialBuffer = serialBuffer.replace(line + '\n', '')
                        }

                    }else if(ontvagAlleenBijNewLine){
                    	var graphHeader = "GRAPH_HEADER:";
	                	if (line.indexOf(graphHeader) != -1) {
    	            		var headers = line.substr(line.indexOf(graphHeader) + graphHeader.length);
    	            		plotterTitles = headers.split(',');
        	        	}
                        // if(showTimeStamp){line = tijdString() + " > " + line}//-----------------tijd stamp
                        if(showTimeStamp){line = sampleIndex + " > " + line}//-----------------sample stamp
                        serialBuffer += line + '\n';
                    }
                }


                if(!ontvagAlleenBijNewLine){
                    // if(showTimeStamp){
                    //     value.split('\n').join("\nhallo> ");
                    // }
                    serialBuffer += value;//---------------------------------------------ontvangen text
                }

                

                if(ontvangen.textContent.length != serialBuffer.length){//----------------automatisch scrollen
                    ontvangen.textContent = serialBuffer
                    if(autoScroll){
                        ontvangen.scrollTop = ontvangen.scrollHeight;
                    }
                }

                

                
                


            }
            if (done) {
                console.log('[serialRead] DONE', done);
                reader.releaseLock();
                break;
            }
        }
    }catch(err){
        console.log("poort verloren");
        reader.releaseLock();
        port = null;
    }
}











function SerialWrite(schrijfData) {

    
    if(sendCariageReturn){ schrijfData += '\r'}
    if(sendNewLine){ schrijfData += '\n'}
    

    const writer = outputStream.getWriter();
    //console.log('[SEND]', schrijfData);
    writer.write(schrijfData);
    writer.releaseLock();
}














async function connect() {
    // clearPlotter();

    port = await navigator.serial.requestPort();

    var baudrate = parseInt( $("#nbrBaudRate")[0].value )
    // await port.open({ baudRate: 115200 });
    await port.open({ baudRate: baudrate > 10 ? baudrate : 115200 });

    // CODELAB: Add code setup the output stream here.
    const encoder = new TextEncoderStream();
    outputDone = encoder.readable.pipeTo(port.writable);
    outputStream = encoder.writable;


    // CODELAB: Add code to read the stream here.
    let decoder = new TextDecoderStream();
    inputDone = port.readable.pipeTo(decoder.writable);
    inputStream = decoder.readable
    
    reader = inputStream.getReader();
    serialRead();
}











async function disconnect() {

    // CODELAB: Close the input stream (reader).
    if (reader) {
        await reader.cancel();
        await inputDone.catch(() => {});
        reader = null;
        inputDone = null;
    }

    // CODELAB: Close the output stream.
    if (outputStream) {
        await outputStream.getWriter().close();
        await outputDone;
        outputStream = null;
        outputDone = null;
    }

    // CODELAB: Close the port.
    await port.close();
    port = null;
}








async function clickConnect() {
    if (port) {
        await disconnect();
    }else{
        try {
            await connect();
        } catch (error) {
            console.log(error)
            console.log("niet kunnen aansluiten")
            await disconnect();
        }
    }
}
















function clearPlotter(){
    plotterDataPunten = []; // dataPoints
    plotterData = [];
    sampleIndex = 0;
    while(tabel.rows.length > 1){tabel.rows[1].remove()} //verwijder alle data behalve de titel
    updatePlotter()
}













function lijnInfo(arr, tabb) {
    var len = arr.length;
    var min = Infinity; 
    var max = -Infinity;
    var gem = 0;
    var p2p = 0;

    while (len--) {
        
        if(arr[len][1] > max) {
        max = arr[len][1];
        }

        if(arr[len][1] < min) {
            min = arr[len][1];
        }

        gem += arr[len][1];
    }

    gem /= arr.length;
    p2p = max - min;

    
    tabb[1].innerHTML = min
    tabb[2].innerHTML = max
    tabb[3].innerHTML = nummerMaxDecimalen(p2p, 3)
    tabb[4].innerHTML = nummerMaxDecimalen(gem, 3)
    tabb[5].innerHTML = arr[arr.length-1][1]
};






function tijdString(){
    var tijd = new Date()
    // return Math.floor((Date.now()/3600000)%24).pad(2) +":"+ Math.floor((Date.now()/60000)%60).pad(2) +":"+ Math.floor((Date.now()/1000)%60).pad(2) +"."+ (Date.now()%1000).pad(3)
    return tijd.getHours().pad(2) +":"+ tijd.getMinutes().pad(2) +":"+ tijd.getSeconds().pad(2) +"."+ tijd.getMilliseconds().pad(3)

}









function nummerMaxDecimalen(num, x){
    return Math.round(num * Math.pow(10,x)) / Math.pow(10,x)
}







function nummerLengte(num, decimaal, lengte){
    var str = "" + num.toFixed(decimaal)// + ','
    var leeg = lengte - str.length
    while(leeg--){
        str += "&nbsp;";
    }
    return str
}




Number.prototype.pad = function(size) {
    var s = String(this);
    while (s.length < (size || 2)) {s = "0" + s;}
    return s;
}








function byteToHexString(uint8arr) {
    if (!uint8arr) {
        return '';
    }
    
    var hexStr = '';
    for (var i = 0; i < uint8arr.length; i++) {
        var hex = (uint8arr[i] & 0xff).toString(16);
        hex = (hex.length === 1) ? '0' + hex : hex;
        hexStr += hex;
    }
    
    return hexStr.toUpperCase();
}







function hexStringToByte(str) {
    if (!str) {
        return new Uint8Array();
    }

    var a = [];
    for (var i = 0, len = str.length; i < len; i+=2) {
        a.push(parseInt(str.substr(i,2),16));
    }

    return new Uint8Array(a);
}






function nieuweGrafiek(i){
    plotterDataPunten[i] = [];
    plotterData[i] = { data: plotterDataPunten[i], xaxis: 1, yaxis:1, color: plotterKleuren[i], lines: { show: true}}
    
    var tabb = tabel.insertRow(-1);
    tabb.style = "color: " + plotterData[i].color;
    plotterTabel[i] = [];

    for(var c = 0; c < 6; c++){
        plotterTabel[i][c] = tabb.insertCell(-1);
    }
    
    plotterTabel[i][0].innerHTML = 
    // "<input type='checkbox' id='cbxLijn" +i+ "' class='cbx' onclick='if(this.checked){\
    //     plotterData[" +i+ "].data = plotterDataPunten[" +i+ "]\
    // }else{\
    //     plotterData[" +i+ "].data = [];\
    // } lijnToggle = true' checked = 'true'> 
    plotterTitles[i];

    tabb.index = i;
    tabb.checked = true;

    tabb.onclick = function(){ 
        bbb = this
        this.checked = !this.checked

        if(this.checked){
            plotterData[this.index].data = plotterDataPunten[this.index]
            this.style.opacity = '100%'
        }else{
            plotterData[this.index].data = [];
            this.style.opacity = '50%'
        }
        lijnToggle = true
    }
}





function testGrafiek(){
    clearPlotter()
    
    nieuweGrafiek(0)
    nieuweGrafiek(1)
                                    

    for(var i = 0; i < plotterSampleNummer; i++){
        plotterDataPunten[0].push( [i,   Math.sin((i / plotterSampleNummer)* 4 * Math.PI)]  );
        plotterDataPunten[1].push( [i,   Math.cos((i / plotterSampleNummer)* 4 * Math.PI)]  );
    }

    plot.setData(plotterData);
    plot.setupGrid(true);
    plot.draw();
}








// document.onkeypress = function(e){
//     if(document.hasFocus()){
//         console.log("halloooooo")
//     }
//     console.log(e.key)
// }

