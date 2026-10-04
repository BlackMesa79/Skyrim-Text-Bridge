#pragma once
inline constexpr char MeridianScript[] = R"STBJS((function(){
    if (window.__stbMeridian) return;
    function active() {
        var d=document,e=d.activeElement;
        while(e && e.tagName==='IFRAME') { d=e.contentDocument; if(!d)return null; e=d.activeElement; }
        if(!e || e.disabled || e.readOnly || e.type==='password')return null;
        if(e.tagName!=='TEXTAREA' && !(e.tagName==='INPUT' && /^(text|search|email|url|tel)$/.test(e.type||'text')))return null;
        return {doc:d,element:e};
    }
    var state=null;
    var observer=0,nextField=0,fieldKey=0,lastField=null;
    function probe(id) {
        if(!observer || observer!==id)return;
        var field=null;
        try { if(!document.hasFocus || document.hasFocus())field=active(); } catch(_) {}
        var key=0;
        var element=field ? field.element : null;
        if(element!==lastField) {lastField=element;fieldKey=element ? ++nextField : 0;}
        key=fieldKey;
        if(typeof window.stbMeridianFocus==='function')window.stbMeridianFocus(String(id)+':'+String(key));
    }
    function focusChanged(){probe(observer);}
    if(document.addEventListener) {
        document.addEventListener('focusin',focusChanged,true);
        document.addEventListener('focusout',function(){lastField=null;fieldKey=0;if(observer && typeof window.stbMeridianFocus==='function')window.stbMeridianFocus(String(observer)+':0');},true);
    }
    window.addEventListener('blur',function(){state=null;lastField=null;fieldKey=0;if(observer && typeof window.stbMeridianFocus==='function')window.stbMeridianFocus(String(observer)+':0');});
    function report(id,status) { if(typeof window.stbMeridianResult==='function') window.stbMeridianResult(String(id)+':'+status); }
    window.__stbMeridian={
        observe:function(id){observer=id;lastField=null;fieldKey=0;probe(id);},
        probe:probe,
        stopObserve:function(id){if(observer===id){observer=0;lastField=null;fieldKey=0;}},
        capture:function(id,expectedField) {
            state=null;
            try { var field=active(); if(field && (!expectedField || (observer && field.element===lastField && fieldKey===expectedField))) {state={id:id,doc:field.doc,element:field.element};report(id,'ready');} else report(id,'no-field'); }
            catch(_) {report(id,'capture-error');}
        },
        commit:function(id,text) {
            try {
                var field=active();
                if(!state || state.id!==id || !field || field.element!==state.element || !state.doc.documentElement.contains(state.element)) {report(id,'stale-field');return;}
                // Native edit operation emits the input event Tailor search/name fields use.
                if(!state.doc.execCommand('insertText',false,text)) {report(id,'insert-rejected');return;}
                report(id,'inserted');
            } catch(_) {report(id,'insert-error');}
        },
        cancel:function(id) {if(state && state.id===id)state=null;}
    };
})()
)STBJS";
