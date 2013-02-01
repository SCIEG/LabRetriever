function copy(obj, depth) {
    var newObj = (obj instanceof Array) ? [] : {};
    for (i in obj) {
        if (obj[i] && typeof obj[i] == "object" && depth) {
            newObj[i] = copy(obj[i], depth-1);
        } else newObj[i] = obj[i];
    } return newObj;
};


function displayData() {
    var addedColumn = 0;
    $("#locusTable tr").each(function(idx, value) {
        var samples = SCIEG.selectedSamples[SCIEG.activeColumn];
        var row = $(value);
        var cols = row.find('td');
        var colIdx = SCIEG.activeColumn + samples.length - 1;
        for (var i = SCIEG.activeColumn - 1; i > 0; i--) {
            colIdx += Math.max(0, SCIEG.selectedSamples[i].length - 1);
        }

        if (idx < 2) {
            if (samples.length > 1) {
                var html = "<td></td>";
                if (idx == 0) {
                    html = "<td>" + (SCIEG.activeColumn == 1? 'Assumed': 'Suspected') + "</td>";
                }
                $(cols[colIdx-1]).after(html);
                addedColumn = 1;
            }
            return;
        }

        cols = addColumns(cols, colIdx + 1);

        if (addedColumn) {
            $(cols[colIdx-1]).after("<td></td>");
            cols = row.find('td');
        }

        var sample = samples[samples.length - 1];
        var display = idx == 2 ? sample['name'] :
            (value.id in sample)? sample[value.id].join(' ') : '';

        $(cols[colIdx]).html(display);
        if (idx == 2) {
            $(cols[colIdx]).addClass("sampleName");
        }

        $('.sampleName').mouseenter(showRemove).mouseleave(hideRemove);
    });
}

function calculateUnattributed() {
    var maxcols = 0;
    $("#locusTable tr").each(function(idx, value) {
        if (idx == 0) {
            maxcols = $(value).find('td').length;
        }
        var cols = $(value).find('td');
        cols = addColumns(cols, maxcols);
        if (idx < 3) return;

        var all = (SCIEG.selectedSamples[3].length && (value.id in SCIEG.selectedSamples[3][0])) ?
            copy(SCIEG.selectedSamples[3][0][value.id], 1) : [];
        $.each(SCIEG.selectedSamples[1], function(i, v){
            if (!(value.id in v)) return;
            $.each(v[value.id], function(ii, vv){
                if (all.indexOf(vv) != -1) {
                    all.splice(all.indexOf(vv), 1);
                }
            });
        })
        $(cols[maxcols - 1]).html(all.join(" "));
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
        rm = el.append('<div class="removeSample hidden" onclick="removeSample(this)">remove</div>');
        var pos = el.position();
        rm.css({'top': pos.top + (el.height() / 2), 'left': pos.left + (el.width() / 2)});
    } else {
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

    var idx = SCIEG.selectedSamples[1].length || 1;
    if (col <= idx) {
        removeColumn(col, SCIEG.selectedSamples[1].length == 1 || col == 1, SCIEG.selectedSamples[1].length > 1);
        var sample = SCIEG.selectedSamples[1].pop();
        SCIEG.usedSamples.splice(SCIEG.usedSamples.indexOf(sample['name']), 1);
        // Assumed, need to recalculate unattributed
        calculateUnattributed();
        updateSampleSelect()
        return;
    }
    idx += SCIEG.selectedSamples[2].length || 1;
    if (col <= idx) {
        removeColumn(col, SCIEG.selectedSamples[2].length == 1 || col == Math.max(1, SCIEG.selectedSamples[1].length) + 1,
            SCIEG.selectedSamples[2].length > 1);
        var sample = SCIEG.selectedSamples[2].pop();
        SCIEG.usedSamples.splice(SCIEG.usedSamples.indexOf(sample['name']), 1);
        updateSampleSelect();
        return;
    }
    idx += 1;
    if (col <= idx) {
        try {
        removeColumn(col, true, false);
        var sample = SCIEG.selectedSamples[3].pop();
        SCIEG.usedSamples.splice(SCIEG.usedSamples.indexOf(sample['name']), 1);
        // show the + icon again
        $($('#locusTable .addSample')[2]).css('visibility', 'visible');
        calculateUnattributed();
        updateSampleSelect();
        } catch(e){log('removal exception: ' + e.toString())}
        return;
    }
}

function removeColumn(idx, keepHeader, shiftCells) {
    $("#locusTable tr").each(function(i, value) {
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
    // assumed
    1: [],
    // suspected
    2: [],
    // evidence
    3: []
}
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
            if (SCIEG.activeColumn == 3) {
                $($('#locusTable .addSample')[2]).css('visibility', 'hidden');
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
    if (col > 1) col -= Math.max(SCIEG.selectedSamples[1].length - 1, 0);
    if (col > 2) col -= Math.max(SCIEG.selectedSamples[2].length - 1, 0);
    SCIEG.activeColumn = col;
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
    $('#locusTable .addSample').each(function(idx, value){
        $(value).html("<a href='#' onclick='chooseSample(this)'><img src='img/plus.png'/></a>");
    });
    updateSampleSelect();
    $('#loadFile').html("Load another file");
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