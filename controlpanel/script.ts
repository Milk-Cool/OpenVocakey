const voices = document.querySelector("#voices") as HTMLDivElement;
const songs = document.querySelector("#songs") as HTMLDivElement;
const connect = document.querySelector("#connect") as HTMLButtonElement;

const encoder = new TextEncoder();
const decoder = new TextDecoder();

let writer: WritableStreamDefaultWriter, reader: ReadableStreamDefaultReader;

const exec = async (cmd: string, err: string) => {
    await writer.write(encoder.encode(cmd));
    let str = "";
    while(!str.endsWith("> ")) {
        const { value, done} = await reader.read();
        if(done) throw new Error(err);
        str += decoder.decode(value).replaceAll("\r", "");
    }
    return str.replace(/> $/, "");
};
const upd = async (prefix: string, cmd: string, err: string) => {
    return (await exec(cmd, err)).split("\n").filter(x => x.startsWith(prefix)).map(x => x.replace(prefix, ""));
};
const dl = (txt: string, name: string) => {
    const blob = new Blob([txt], { type: "text/plain" });
    const url = URL.createObjectURL(blob);
    const a = document.createElement("a")
    a.href = url;
    a.download = name;
    a.click();
    setTimeout(() => {
        a.remove();
        URL.revokeObjectURL(url);
    }, 1000);
}
const downloadVoice = async (name: string) => {
    const txt = await exec("v_get " + name + "\n", "couldn't get voice!");
    await dl(txt, name);
};
const downloadSong = async (name: string) => {
    const txt = await exec("s_get " + name + "\n", "couldn't get voice!");
    await dl(txt, name);
};
const createListItem = (name: string, onClick: () => any) => {
    const el = document.createElement("div");
    el.classList.add("listitem");
    el.innerText = name;
    el.addEventListener("click", onClick);
    return el;
};
const updVoices = async () => {
    const list = await upd("/vbs/", "v_list\n", "couldn't fetch voices!");
    for(const el of Array.from(document.querySelector("#voicelist")!.children))
        el.remove();
    for(const f of list)
        document.querySelector("#voicelist")!.appendChild(createListItem(f, () => downloadVoice(f)));
};
const updSongs = async () => {
    const list = await upd("/songs/", "s_list\n", "couldn't fetch songs!");
    for(const el of Array.from(document.querySelector("#songlist")!.children))
        el.remove();
    for(const f of list)
        document.querySelector("#songlist")!.appendChild(createListItem(f, () => downloadSong(f)));
};
const upload = async (cmd: string, text: string) => {
    await writer.write(encoder.encode(cmd));
    await writer.write(encoder.encode(text));
    await writer.write(encoder.encode("EOF\n"));
};
const rfile = async (f: File) => {
    return await new Promise(resolve => {
        const freader = new FileReader();
        freader.onload = e => {
            resolve(e.target!.result as string);
        };
        freader.readAsText(f);
    }) as string;
}
connect.addEventListener("click", async () => {
    try {
        // @ts-ignore // too lazy to figure out how to install types
        const port = await navigator.serial.requestPort();
        await port.open({ baudRate: 115200 });
        reader = (port.readable as ReadableStream).getReader();
        writer = (port.writable as WritableStream).getWriter();

        await updVoices();
        await updSongs();

        document.querySelector("#voiceupload")?.addEventListener("click", async () => {
            const f = (document.querySelector("#voicefile") as HTMLInputElement).files![0];
            const txt = await rfile(f);
            await upload("v_set " + ((document.querySelector("#voicename") as HTMLInputElement).value || "voice.txt") + "\n", txt);
            await updVoices();
            alert("success!");
        });
        document.querySelector("#songupload")?.addEventListener("click", async () => {
            const f = (document.querySelector("#songfile") as HTMLInputElement).files![0];
            const txt = await rfile(f);
            await upload("s_set " + ((document.querySelector("#songname") as HTMLInputElement).value || "voice.txt") + "\n", txt);
            await updSongs();
            alert("success!");
        });
    } catch(e) {
        alert(e);
        console.error(e);
    }
});