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
    window.addEventListener('blur',function(){state=null;});
    function report(id,status) { if(typeof window.stbMeridianResult==='function') window.stbMeridianResult(String(id)+':'+status); }
    window.__stbMeridian={
        capture:function(id) {
            state=null;
            try { var field=active(); if(field) {state={id:id,doc:field.doc,element:field.element};report(id,'ready');} else report(id,'no-field'); }
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
