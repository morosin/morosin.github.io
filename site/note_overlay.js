let overlayNoteId = null;
let overlayContainer = null;
function overlayClose() {
    overlayContainer?.hidePopover();
    overlayContainer?.replaceChildren();
    overlayNoteId = null;
}
function overlayCreateNote(id) {
    let wrapper = undefined;
    document.querySelectorAll("aside.footnotetext").forEach(aside => {
        const bklink = aside.querySelector(".footnote-mark a");
        const noteid = bklink?.id;
        if (noteid && noteid === id) {
            const clone = aside.cloneNode(true);
            wrapper = document.createElement("div");
            wrapper.className = "note-wrapper";
            wrapper.appendChild(clone);
        }
    });
    return wrapper;
}
function overlayOpen(id) {
    const notetext = overlayCreateNote(id);
    if (!notetext) {
        console.warn(`missing footnote ${id}`);
        return;
    }
    overlayNoteId = id;
    overlayContainer?.appendChild(notetext);
    overlayContainer?.showPopover();
}
function overlaySetup() {
    overlayContainer = document.querySelector(".note-overlay");
    overlayContainer?.addEventListener("toggle", (ev) => {
        if (ev.newState === "closed") {
            overlayContainer?.replaceChildren();
            overlayNoteId = null;
        }
    });
    const anchors = document.querySelectorAll("body :not(aside) .footnote-mark a");
    anchors.forEach(anchor => {
        const id = anchor.getAttribute("href")?.substring(1);
        if (id && id.startsWith("fn") && !id.endsWith("-bk")) {
            anchor.addEventListener("click", (ev) => {
                overlayOpen(id);
                ev.preventDefault();
                return false;
            });
        }
    });
}
window.addEventListener("load", () => overlaySetup());
