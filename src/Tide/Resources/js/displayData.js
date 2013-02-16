function copy(obj, depth) {
    var newObj = (obj instanceof Array) ? [] : {};
    for (i in obj) {
        if (obj[i] && typeof obj[i] == "object" && depth) {
            newObj[i] = copy(obj[i], depth-1);
        } else newObj[i] = obj[i];
    } return newObj;
};

String.prototype.capitalize = function() {
    return this.charAt(0).toUpperCase() + this.slice(1);
}

function displayData() {
    var addedColumn = 0;
    $("#inputs tr").each(function(idx, value) {
        var samples = SCIEG.selectedSamples[SCIEG.activeColumn];
        var row = $(value);
        var cols = row.find('td');
        var colIdx = SCIEG.colMap[SCIEG.activeColumn] + samples.length - 1;
        if (SCIEG.activeColumn == 'suspected') {
            colIdx += Math.max(0, SCIEG.selectedSamples['assumed'].length - 1);
        }

        if (idx < 2) {
            if (samples.length > 1) {
                var html = "<td></td>";
                if (idx == 0) {
                    html = "<td>" + SCIEG.activeColumn.capitalize() + "</td>";
                }
                $(cols[colIdx-1]).after(html);
                addedColumn = 1;
            }
            return;
        }

        cols = addColumns(cols, colIdx);

        if (addedColumn) {
            $(cols[colIdx-1]).after("<td></td>");
            cols = row.find('td');
        }

        var sample = samples[samples.length - 1];
        if (idx == 2) {
            var sampleNameId = 'editable' + sample['name'].replace(/ /g, '').replace(/\|/g, '');
            var display = '<span data-type="editable" data-for="#' + sampleNameId + '">' +
                sample['name'] + '</span><input type="text" style="display:none;" id="' + sampleNameId + '"/>';

            $(cols[colIdx]).html(display).addClass("sampleName").editables({
                beforeFreeze: function(l){if (this.val()) l.text(this.val());},
                beforeEdit: function(t){t.val(this.text());hideRemove.call(this.parent());}
            });
            $('.sampleName').mouseenter(showRemove).mouseleave(hideRemove);
        } else {
            var display = (value.id in sample)? sample[value.id].join('&nbsp;') : '';
            $(cols[colIdx]).html(display);
        }
    });
}

function displayOutput(data) {

    var dataSize = 0;
    var keys = 0;
    races = ["AFRICAN_AMERICAN", "CAUCASIAN", "HISPANIC"]
    for (r in data) {
        keys++;
    }
    $("#result tr").each(function(idx, value) {
        if (idx == 0) {
            return;
        }
        try {
        var row = $(value);
        var cols = row.find('td');
        cols = addColumns(cols, keys + 1);


        $.each(races, function(i,v){
            if (data[v])
                cols[1 + i].innerHTML = idx == 1 ? v : data[v][value.id] || '';
        });

        } catch (e) { log(e.toString()); }
    });
    $('#result').removeClass("hidden");
    var tds = $('#inputs td');
    var width = $(tds[0]).width() + $(tds[1]).width() + 35;
    var origWidth = $('#inputs').width();
    $('#inputs').css({'overflow': 'hidden', 'width': width});
    $($('#result tr')[2]).find('td').css('height', $($('#inputs tr')[2]).find('td').height());
    if ($('#expandInputs').length == 0) {
        var b = document.createElement('span');
        b.className = 'button';
        b.innerHTML = '&gt;&gt;';
        b.id = 'expandInputs';
        b.style.marginLeft = '5px';
        $('#inputs tr.names td')[1].appendChild(b);
        $('#expandInputs').click(function(){
            var ei = $('#expandInputs');
            if (ei.html() == '&gt;&gt;') {
                ei.html('&lt;&lt;');
                $('#locusTable').css('width', origWidth + $('#result').width() + 100);
                $('#inputs').animate({'width':origWidth}, 1500);
            } else {
                ei.html('&gt;&gt;');
                $('#inputs').animate({'width':width}, 1500);
            }
        });
    } else {
        $('#expandInputs').html('&gt;&gt;');
    }
}

function calculateUnattributed() {
    var maxcols = 0;
    $("#inputs tr").each(function(idx, value) {
        if (idx == 0) {
            maxcols = $(value).find('td').length;
        }
        var cols = $(value).find('td');
        cols = addColumns(cols, maxcols);
        if (idx < 3) return;

        var all = (SCIEG.selectedSamples['detected'].length && (value.id in SCIEG.selectedSamples['detected'][0])) ?
            copy(SCIEG.selectedSamples['detected'][0][value.id], 1) : [];
        $.each(SCIEG.selectedSamples['assumed'], function(i, v){
            if (!(value.id in v)) return;
            $.each(v[value.id], function(ii, vv){
                if (all.indexOf(vv) != -1) {
                    all.splice(all.indexOf(vv), 1);
                }
            });
        })
        $(cols[2]).html(all.join(" "));
    });
}

function addColumns(cols, size) {
    for (var i=0; i < size - cols.length; i++) {
        $(cols[cols.length-1]).after("<td></td>");
    }
    return $(cols[0].parentNode).find('td');
}

function showRemove(evt) {
    var el = $(this);
    var rm = el.find('.removeSample');
    if (rm.length == 0) {
        el.append('<div class="removeSample" onclick="removeSample(this)">remove</div>');
        var pos = el.position();
        rm = el.find('.removeSample');
        rm.css({'top': pos.top /* + (el.height() / 2)*/, 'left': pos.left + el.width()});
    } else {
        if (el.find('input').css('display') == 'none')
            rm.removeClass('hidden');
    }
}

function hideRemove() {
    $(this).find('.removeSample').addClass('hidden');
}

function removeSample(el) {
    var cell = $(el).parent();
    var col = 0;
    while (cell.prev().length) {
        col += 1;
        cell = cell.prev();
    }
    var sampleCol = col;
    if (sampleCol > 3) {
        sampleCol -= Math.max(SCIEG.selectedSamples[SCIEG.colMap[3]].length - 1, 0);
        if (sampleCol > 4) sampleCol = 4;
    }

    if (SCIEG.colMap[sampleCol] == 'detected' || SCIEG.colMap[sampleCol] == 'suspected') {
        removeColumn(col, true, false);
        // show the + icon again
        $($('#inputs .addSample a')[SCIEG.activeColumn == 'detected'?0:2]).css('visibility', 'visible');
    } else {
        var onHeaderColumn = sampleCol == col ||
            (sampleCol == 4 && col == 3 + SCIEG.selectedSamples[SCIEG.colMap[3]].length);
        removeColumn(col, onHeaderColumn,
            SCIEG.selectedSamples[SCIEG.colMap[sampleCol]].length > 1);
    }

    var sample = SCIEG.selectedSamples[SCIEG.colMap[sampleCol]].pop();
    SCIEG.usedSamples.splice(SCIEG.usedSamples.indexOf(sample['name']), 1);
    if (SCIEG.colMap[sampleCol] != 'suspected') calculateUnattributed();
    updateSampleSelect();
}

function removeColumn(idx, keepHeader, shiftCells) {
    $("#inputs tr").each(function(i, value) {
        var cols = $(value).find('td');
        if (keepHeader) {
            if (i < 2) {
                if (shiftCells) {
                    cols[idx+1].parentNode.removeChild(cols[idx+1]);
                }
                return;
            }
            if (i == 2) {
                $(cols[idx]).unbind('mouseenter').unbind(['mouseleave','sampleName']);
            }
            if (shiftCells) {
                cols[idx].parentNode.removeChild(cols[idx]);
            } else {
                cols[idx].innerHTML = '';
            }
        } else {
            cols[idx].parentNode.removeChild(cols[idx]);
        }
    });
}

SCIEG.usedSamples = [];
SCIEG.selectedSamples = {
    'assumed': [],
    'suspected': [],
    'detected': []
};
SCIEG.colMap = {
    3: 'assumed',
    'assumed': 3,
    4: 'suspected',
    'suspected': 4,
    1: 'detected',
    'detected': 1
};
SCIEG.activeColumn = 0;

function selectFile() {
    removeOverlay();
    SCIEG.usedSamples.push(this.innerHTML);
    try {
    updateSampleSelect();

    for (var i=0; i < SCIEG.fileData.length; i++) {
        if (SCIEG.fileData[i]['name'] == this.innerHTML) {
            SCIEG.selectedSamples[SCIEG.activeColumn].push(SCIEG.fileData[i]);
            displayData();
            calculateUnattributed();
            if (SCIEG.activeColumn == 'detected' || SCIEG.activeColumn == 'suspected') {
                $($('#inputs .addSample a')[SCIEG.activeColumn == 'detected'?0:2]).css('visibility', 'hidden');
            }
            break;
        }
    }
    } catch (e) {log("selectFile exception: " + e.toString())}
}

function removeOverlay() {
    $('#sampleSelector').addClass("hidden");
    $('#overlay').addClass("hidden");
    $('#results').addClass('hidden');
}

function chooseSample(el) {
  try{
    var ss = $('#sampleSelector');
    ss.removeClass("hidden");
    $('#overlay').removeClass("hidden").click(removeOverlay);
    var cell = $(el).parent();
    var position = cell.position();
    var containerPos = $("#locusTable").position();
    ss.css({'top':position.top + containerPos.top, 'left':position.left + containerPos.left});
    var col = 0;
    while (cell.prev().length) {
        col += 1;
        cell = cell.prev();
    }
    if (col > 3) col -= Math.max(SCIEG.selectedSamples[SCIEG.colMap[3]].length - 1, 0);
    if (col > 4) col = 4;
    SCIEG.activeColumn = SCIEG.colMap[col];
  } catch (e) { log("chooseSample exception: " + e.toString());}
}

function updateSampleSelect() {
    $('#sampleSelector .fileSelect').off();
    var list = document.createElement("ul");
    list.className = 'fileSelect';
    var heading = document.createElement("h4");
    heading.innerHTML = 'pick a sample';
    $.each(SCIEG.fileData, function(idx, val) {
        if (SCIEG.usedSamples.indexOf(val['name']) >= 0) return;
        var opt = document.createElement("li");
        opt.innerHTML = val['name'];
        list.appendChild(opt);
    });
    $('#sampleSelector').html(heading).append(list);
    $('#sampleSelector .fileSelect li').click(selectFile);
}

function fileLoaded() {
    $('#inputs .addSample').each(function(idx, value){
        $(value).html("<a href='#' onclick='chooseSample(this)'><img src='img/plus.png'/></a>");
    });
    updateSampleSelect();
    $('#loadFile').html("Load another file");
    $('#runner').removeClass('hidden');
    SCIEG.util.status('file loaded.');
}

function addAnotherDO(e) {
    e.preventDefault();
    var parent = $('#addDO').parent();
    var input = parent.prev().find('input');
    var val = parseFloat(input.val());
    if (!val || val > 1 || val < 0) {
        alert("Please enter a value between 0 and 1 for prior DO before adding another.");
        input.focus();
        return;
    }
    var li = document.createElement("li");
    li.innerHTML = 'P(DO) <input type="text" name="pdo" class="quarter" value=""/>';
    parent.before(li);
}