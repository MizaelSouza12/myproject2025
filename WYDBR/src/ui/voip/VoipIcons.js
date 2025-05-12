/**
 * VoipIcons
 * 
 * Fornece ícones SVG para a interface VOIP.
 * Todos os ícones são implementados como Base64 ou SVG inline para evitar
 * dependências externas e problemas de carregamento.
 */

// Objeto de ícones
const VoipIcons = {
  // Ícone de microfone
  mic: 'data:image/svg+xml;base64,PHN2ZyB4bWxucz0iaHR0cDovL3d3dy53My5vcmcvMjAwMC9zdmciIHZpZXdCb3g9IjAgMCAyNCAyNCIgZmlsbD0id2hpdGUiPjxwYXRoIGQ9Ik0xMiAxNGMxLjY2IDAgMy0xLjM0IDMtM1Y1YzAtMS42Ni0xLjM0LTMtMy0zUzkgMy4zNCA5IDV2NmMwIDEuNjYgMS4zNCAzIDMgM3ptNS45MS0zYy0uNDkgMC0uOS4zNi0uOTguODVDMTYuNTIgMTQuMiAxNC40NyAxNiAxMiAxNmMtMi40NyAwLTQuNTItMS44LTQuOTMtNC4xNS0uMDgtLjQ5LS40OS0uODUtLjk4LS44NS0uNjEgMC0xLjA5LjU0LTEgMS4xNC40OSAzIDIuODkgNS4zNSA1LjkxIDUuNzhWMjBjMCAuNTUuNDUgMSAxIDFzMS0uNDUgMS0xdi0yLjA4YzMuMDItLjQzIDUuNDItMi43OCA1LjkxLTUuNzguMS0uNi0uMzktMS4xNC0xLTEuMTR6Ii8+PC9zdmc+',
  
  // Ícone de microfone mudo
  micMute: 'data:image/svg+xml;base64,PHN2ZyB4bWxucz0iaHR0cDovL3d3dy53My5vcmcvMjAwMC9zdmciIHZpZXdCb3g9IjAgMCAyNCAyNCIgZmlsbD0id2hpdGUiPjxwYXRoIGQ9Ik0xOSAxMWgtMS4xNmMtLjExLjUtLjMgMS0uNTQgMS40NGw4LjQxIDQuMzRMLjU0IDEuODggMi4xOCAzLjUyQzEuOTUgNC4yNSAxLjc3IDUuMDEgMS43NSA1LjgxdjUuMTljMCA0LjgzIDMuMTMgOS43OCA4IDEwLjVWMjNoMnYtMS41Yy40Ny0uMDggLjkyLS4yMSAxLjM3LS4zN2wxLjg4IDEuODggMS42NS0xLjY1LTIuODktMi44OGMtLjI3LS4yNy0uNzMtLjI3LTEgME0xMiAxOWMtMy44NSAwLTctMy4xMy03LTd2LTJjMC0uNS4wNC0xLjAxLjE1LTEuNWwxMS42MyAxMS42M2MtMS41LjU3LTMuMDggLjg3LTQuNzguODdtNi4xNy0xLjE3QzE2Ljc0IDE3LjkxIDE0LjQ0IDE4LjI1IDEyIDE4LjI1YzItLjUxIDMuOC0xLjQgNS4xNy0yLjY1bTEtMy42M2wuOTIuOTJjLjA1LS4yNy4wOC0uNTUuMDgtLjgzdi0ySDEzdjJjMCAuMDM2LS4wMDMuMDcxLS4wMDYuMTA2TTEyIDRjMS42NyAwIDMgMS4zNCAzIDN2Ni42M2w0LjAxIDQuMDFjLjQ2LTEuMTguNzctMi40NC44Ni0zLjczSDIxVjdoLTJjLS4wNS0xLjM0LS40LTIuNTgtMS0zLjY3TDEyLjYgOC4zVjdoLTEuMzFMOS40MSA0SDEyeiIvPjwvc3ZnPg==',
  
  // Ícone de volume
  volumeUp: 'data:image/svg+xml;base64,PHN2ZyB4bWxucz0iaHR0cDovL3d3dy53My5vcmcvMjAwMC9zdmciIHZpZXdCb3g9IjAgMCAyNCAyNCIgZmlsbD0id2hpdGUiPjxwYXRoIGQ9Ik0zIDloNGw1LTVWMjBsLTUtNUgzVjlNMTYuNSAxMmMwLTEuNzctLTEtMy4yOS0yLjUtNC4xM3YyVjExLjg3YzAuODIuNTEgMS41IDEuNDUgMS41IDIuNjNzLS42OCAyLjEyLTEuNSAyLjYzdjIuMDFjMS41LS44NCAyLjUtMi4zNiAyLjUtNC4xNG0tMi41IDguMTNjMi4yOS0xLjI5IDQtMy44IDQtNi42M3MtMS43MS01LjM0LTQtNi42M3Y3Ljg3VjIwLjEzIi8+PC9zdmc+',
  
  // Ícone de volume mudo
  volumeMute: 'data:image/svg+xml;base64,PHN2ZyB4bWxucz0iaHR0cDovL3d3dy53My5vcmcvMjAwMC9zdmciIHZpZXdCb3g9IjAgMCAyNCAyNCIgZmlsbD0id2hpdGUiPjxwYXRoIGQ9Ik0xMi44NyA1bC02LjEyIDYuMTF2LTQuMjJsNi4xMi02LjExdjQuMjJNNS44MyA5aDIuODNsLTQuMjcgNC4yN0guMWMtLjQxIDAtLjQxIDAtLjQxLS40MVY5LjQxYzAtLjQxIDAtLjQxLjQxLS40MW01Ljg3LTIuODZ2LTEuMTlsLTYuNjcgNi42OGgxLjE4MTVsNS40ODg1LTUuNDl6Ii8+PC9zdmc+',
  
  // Ícone de configurações
  settings: 'data:image/svg+xml;base64,PHN2ZyB4bWxucz0iaHR0cDovL3d3dy53My5vcmcvMjAwMC9zdmciIHZpZXdCb3g9IjAgMCAyNCAyNCIgZmlsbD0id2hpdGUiPjxwYXRoIGQ9Ik0xOS4xNiAxMi45NGMuMDQtLjMuMDYtLjYxLjA2LS45NCAwLS4zMi0uMDItLjY0LS4wNy0uOTRsMi4wMy0xLjU4Yy4xOS0uMTQuMjQtLjQxLjEyLS42MWwtMS45Mi0zLjMyYy0uMTItLjIyLS4zNy0uMjktLjU5LS4yMmwtMi4zOS45NmMtLjUtLjM4LTEuMDQtLjctMS42Mi0uOTRsLS4zNi0yLjU0Yy0uMDQtLjI0LS4yNC0uNDEtLjQ4LS40MWgtMy44NGMtLjI0IDAtLjQzLjE3LS40Ny40MWwtLjM2IDIuNTRjLS41OS4yNC0xLjEzLjU3LTEuNjIuOTRsLTIuMzktLjk2Yy0uMjItLjA4LS40NyAwLS41OS4yMkwyLjc0IDguODdjLS4xMi4yMS0uMDguNDcuMTIuNjFsMi4wMyAxLjU4Yy0uMDUuMy0uMDkuNjMtLjA5Ljk0cy4wMi42NC4wNy45NGwtMi4wMyAxLjU4Yy0uMTkuMTQtLjIzLjQxLS4xMi42MWwxLjkyIDMuMzJjLjEyLjIyLjM3LjI5LjU5LjIybDIuMzktLjk2Yy41LjM4IDEuMDMuNyAxLjYyLjk0bC4zNiAyLjU0Yy4wNS4yNC4yNC40MS40OC40MWgzLjg0Yy4yNCAwIC40NC0uMTcuNDctLjQxbC4zNi0yLjU0Yy41OS0uMjQgMS4xMy0uNTYgMS42Mi0uOTRsMi4zOS45NmMuMjIuMDguNDcgMCAuNTktLjIybDEuOTItMy4zMmMuMTItLjIyLjA3LS40Ny0uMTItLjYxbC0yLjAxLTEuNTh6TTEyIDE1LjZjLTEuOTggMC0zLjYtMS42Mi0zLjYtMy42czEuNjItMy42IDMuNi0zLjYgMy42IDEuNjIgMy42IDMuNi0xLjYyIDMuNi0zLjYgMy42eiIvPjwvc3ZnPg==',
  
  // Ícone de grupo/sala
  group: 'data:image/svg+xml;base64,PHN2ZyB4bWxucz0iaHR0cDovL3d3dy53My5vcmcvMjAwMC9zdmciIHZpZXdCb3g9IjAgMCAyNCAyNCIgZmlsbD0id2hpdGUiPjxwYXRoIGQ9Ik0xNiAxMWMxLjY2IDAgMi45OS0xLjM0IDIuOTktM1MxNy42NiA1IDE2IDVjLTEuNjYgMC0zIDEuMzQtMyAzczEuMzQgMyAzIDN6bS04IDBjMS42NiAwIDIuOTktMS4zNCAyLjk5LTNTOS42NiA1IDggNUM2LjM0IDUgNSA2LjM0IDUgOHMxLjM0IDMgMyAzem0wIDJjLTIuMzMgMC03IDEuMTctNyAzLjV2Mi41aDE0di0yLjVjMC0yLjMzLTQuNjctMy41LTctMy41em04IDBjLS4yOSAwLS42Mi4wMi0uOTcuMDUgMS4xNi44NCAxLjk3IDEuOTcgMS45NyAzLjQ1VjE5aDZ2LTIuNWMwLTIuMzMtNC42Ny0zLjUtNy0zLjV6Ii8+PC9zdmc+',
  
  // Ícone de amigos
  friends: 'data:image/svg+xml;base64,PHN2ZyB4bWxucz0iaHR0cDovL3d3dy53My5vcmcvMjAwMC9zdmciIHZpZXdCb3g9IjAgMCAyNCAyNCIgZmlsbD0id2hpdGUiPjxwYXRoIGQ9Ik0xNiAxMWMxLjY2IDAgMi45OS0xLjM0IDIuOTktM1MxNy42NiA1IDE2IDVjLTEuNjYgMC0zIDEuMzQtMyAzczEuMzQgMyAzIDN6bS04IDBjMS42NiAwIDIuOTktMS4zNCAyLjk5LTNTOS42NiA1IDggNUM2LjM0IDUgNSA2LjM0IDUgOHMxLjM0IDMgMyAzem0wIDJjLTIuMzMgMC03IDEuMTctNyAzLjV2Mi41aDE0di0yLjVjMC0yLjMzLTQuNjctMy41LTctMy41em04IDBjLS4yOSAwLS42Mi4wMi0uOTcuMDUgMS4xNi44NCAxLjk3IDEuOTcgMS45NyAzLjQ1VjE5aDZ2LTIuNWMwLTIuMzMtNC42Ny0zLjUtNy0zLjV6Ii8+PC9zdmc+',
  
  // Ícone de usuário/pessoa
  person: 'data:image/svg+xml;base64,PHN2ZyB4bWxucz0iaHR0cDovL3d3dy53My5vcmcvMjAwMC9zdmciIHZpZXdCb3g9IjAgMCAyNCAyNCIgZmlsbD0id2hpdGUiPjxwYXRoIGQ9Ik0xMiAxMmMtLjUgMC0xLS4xNy0xLjQxLS41OSAwIDAgMS4wMy4xMyAxLjQ5LS4yOS40NS0uNDMuNDMtMS4zMy40My0xLjMzIDAtLjY5LS41Ni0xLjI1LTEuMjUtMS4yNXMtMS4yNS41Ni0xLjI1IDEuMjVjMCAxLjA0LjgzIDEuODggMS44OCAxLjg4LjM0IDAgLjY3LS4wOS45NS0uMjUuMDcuMDUuMTkuMDUuMTYuMDQtLjU5LjktMS43NiAxLjM5LTMgMS4zOS0xLjkzIDAtMy41LTEuNTctMy41LTMuNVYzLjVDNi41IDIuNjcgNS44MyAyIDUgMnMtMS41LjY3LTEuNSAxLjUuNjcgMS41IDEuNSAxLjVjMSAwIDEuODktLjY2IDIuMTctMS42Ni4xMi44OSA5LjAxLjg5IDkuMTQgMCAuMjggMSAxLjE3IDEuNjYgMi4xNyAxLjY2LjgzIDAgMS41LS42NyAxLjUtMS41UzE5IDIgMTguMTcgMmMtLjgzIDAtMS41LjY3LTEuNSAxLjV2My4yMWMwIDEuOTMtMS41NyAzLjUtMy41IDMuNXMtMy41LTEuNTctMy41LTMuNVY2LjVjMC0uMTQtLjA0LS4yNy0uMS0uMzh2LjI2bC4wNC4wNy4yMy42M2MwIC4wMTUgMCAuMDI2LS4wNCAwIC4xNC0uMjI4LjEtLjI3OS4xNy0uMzU5LjE0LS4xNzEuMjktLjE0MS40MS0uMTQxdi0uM2wwLS4wMyAwLS4wMmMwLS4zODUtLjI5LS43My0uNjctLjgzbC0uMzMtLjA5VjQuMzhsLjA0LS4wNzdjLjEtLjEzNy4yNi0uMDA5LjMtLjExOWwuMDMtLjE5Yy4wMi0uMTIuMDMtLjE3OC4wNC0uMjQxIDAtLjA5IDAtLjA2IDAtLjE5NEwxNSA2ek0yMCA2LjVjLjI4IDAgLjUpLjIyLjUuNXYxNGMwIC4yOC0uMjIuNS0uNS41SDRjLS4yNyAwLS41LS4yMi0uNS0uNXYtMTRjMC0uMjguMjMtLjUuNS0uNWgzLjVjMC0uMjcuMjItLjUuNS0uNWg4YS41LjUgMCAwMS41LjVoMy41bS0xIDUuNUg1djhoMTR2LTh6bS0yLTNoLTJ2Mkg4VjloLTJWN2gtMXYySDV2NWgxNFY5aC0xVjd6Ii8+PC9zdmc+',
  
  // Ícone de adicionar (mais)
  add: 'data:image/svg+xml;base64,PHN2ZyB4bWxucz0iaHR0cDovL3d3dy53My5vcmcvMjAwMC9zdmciIHZpZXdCb3g9IjAgMCAyNCAyNCIgZmlsbD0id2hpdGUiPjxwYXRoIGQ9Ik0xOSAxM2gtNnY2aC0ydi02SDV2LTJoNlY1aDJ2Nmg2djJaIi8+PC9zdmc+',
  
  // Ícone de fechar/remover (X)
  close: 'data:image/svg+xml;base64,PHN2ZyB4bWxucz0iaHR0cDovL3d3dy53My5vcmcvMjAwMC9zdmciIHZpZXdCb3g9IjAgMCAyNCAyNCIgZmlsbD0id2hpdGUiPjxwYXRoIGQ9Ik0xOSA2LjQxTDE3LjU5IDUgMTIgMTAuNTkgNi40MSA1IDUgNi40MSAxMC41OSAxMiA1IDE3LjU5IDYuNDEgMTkgMTIgMTMuNDEgMTcuNTkgMTkgMTkgMTcuNTkgMTMuNDEgMTJMMTkgNi40MXoiLz48L3N2Zz4=',
  
  // Ícone de chamada
  call: 'data:image/svg+xml;base64,PHN2ZyB4bWxucz0iaHR0cDovL3d3dy53My5vcmcvMjAwMC9zdmciIHZpZXdCb3g9IjAgMCAyNCAyNCIgZmlsbD0id2hpdGUiPjxwYXRoIGQ9Ik0yMC4wMSAxNS4zOGMtMS4yMyAwLTIuNDItLjItMy41My0uNTYtLjM2LS4xMi0uNzQtLjAzLTEuMDEuMjRsLTEuNTcgMS45N2MtMi44My0xLjM1LTUuNDgtNC4wMS02LjktNi44M2wxLjk1LTEuNjZjLjI3LS4yOC4zNS0uNjcuMjQtMS4wMi0uMzctMS4xMS0uNTYtMi4zLS41Ni0zLjUzIDAtLjU1LS40NS0xLTEtMUg0LjE5Yy0uNTUgMC0xIC40NS0xIDEgMCA5LjM5IDcuNjEgMTcgMTcgMTcgLjU1IDAgMS0uNDUgMS0xdi0zLjQ5YzAtLjU1LS40NS0xLTEtMXpNMTkgMTJoMlY4aC00djJoMnYyem0tNi0yVjhoLTJ2NGg0di0yaC0yeiIvPjwvc3ZnPg==',
  
  // Ícone de desligar chamada
  callEnd: 'data:image/svg+xml;base64,PHN2ZyB4bWxucz0iaHR0cDovL3d3dy53My5vcmcvMjAwMC9zdmciIHZpZXdCb3g9IjAgMCAyNCAyNCIgZmlsbD0iI2YwMCI+PHBhdGggZD0iTTEyIDljLTEuNiAwLTMuMTUuMjUtNC42Ljc3djIuOTFoLjAzQzguNTEgMTIuMjUgMTAuMjYgMTIgMTIgMTJzMy40OC4yNSA0LjU3LjY5aC4wM1Y5Ljc3QzE1LjE1IDkuMjUgMTMuNiA5IDEyIDl6bTAgMmMtMS42NiAwLTMuMjIuMjY0LTQuMDguNzI5VjkuNjdjMS4xOS0uNDY2IDIuNzEtLjY3IDQuMDgtLjY3IDEuMzcgMCAyLjkxLjIwNCA0LjA4LjY3djIuMDZDMTUuMjIgMTEuMjY0IDEzLjY2IDExIDEyIDExIi8+PHBhdGggZD0iTTEgNnYxMC43MzZjMCAxLjQ0OCAxLjEyOCAyLjYzIDIuNTMgMi42M2wxMi4wMi4wMDJjMS40IDAgMi41NC0xLjE4NCAyLjU0LTIuNjNWNmwtOSA0LTE1LTR6bTIuODUgMy4yMzRsMTUuMzEtMy4zNTguMjMtLjAzNiA0LjMgMS45NTZ2Mi42N2MtLjkxLS4wNC0xLjgzLS4wNDMtMi43Ni0uMDQzLTUuNDcgMC0xMC4zMiAxLjM3Ni0xNC42NCA0LjE2NHYtNS45MWwyLjQ2LDEuMTY0LjMuMDEzeiIvPjwvc3ZnPg==',
  
  // Ícone de chat
  chat: 'data:image/svg+xml;base64,PHN2ZyB4bWxucz0iaHR0cDovL3d3dy53My5vcmcvMjAwMC9zdmciIHZpZXdCb3g9IjAgMCAyNCAyNCIgZmlsbD0id2hpdGUiPjxwYXRoIGQ9Ik0yMCAySDRjLTEuMSAwLTIgLjktMiAydjE4bDQtNGgxNGMxLjEgMCAyLS45IDItMlY0YzAtMS4xLS45LTItMi0yem0tMiA3SDZWOGgxMnYxem0wLTNINlY1aDEydjEeiIvPjwvc3ZnPg==',
  
  // Ícone de mudo (áudio desligado)
  audioOff: 'data:image/svg+xml;base64,PHN2ZyB4bWxucz0iaHR0cDovL3d3dy53My5vcmcvMjAwMC9zdmciIHZpZXdCb3g9IjAgMCAyNCAyNCIgZmlsbD0id2hpdGUiPjxwYXRoIGQ9Ik03LjgxIDIwSDQuMWEuOTEuOTEgMCAwMS0uOC0uNGMtLjE1LS4yNi0uMDktLjU0LjEyLS43NGw4LjQyLTguNTUgMS4wNiAxLjA2LTMuNDQgMy40N2E3LjAzIDcuMDMgMCAwMC0xLjY1IDQuNTl2Ljk3ek02LjA0IDEzQzUuMiAxMS45MyA0LjcgMTAuNSA0LjcgOS4wM2EuNzUuNzUgMCAwMC0xLjUgMGMwIDEuODcuNjMgMy42NyAxLjc2IDUuMDlsMS4wOS0xLjEyek0xOS4yIDEyLjMxYy4zNS0uOTguNTQtMi4wNS41NC0zLjIzYS43NS43NSAwIDAxMS41IDAgMTIuMjYgMTIuMjYgMCAwMS0uNyA0LjE1bC0xLjM0LS45MnpNNy4zIDEzLjc1QTUuMjYgNS4yNiAwIDAwOS4wNCAxNmwtMS4yNiAxLjI3YTYuODIgNi44MiAwIDAxLTIuMTktMi44MmwxLjcxLS43ek0xMC4wOCAxOS4xNmE4LjYgOC42IDAgMDA2LjM2LTIuNzkgMTAuMjYgMTAuMjYgMCAwMDEuMzQtMS44MnYuMTljMCAxLjgtMS41NSAzLjI2LTMuNDQgMy4yNmgtNC4yNnptMS40NS0xMC43NGMwLTEuMi45Ny0yLjE2IDIuMTYtMi4xNi4yIDAgLjQuMDEuNTguMGwxLjI0IDEuMjNjLS4wMy0uMDEtLjA1LS4wMS0uMDgtLjAxYTEgMSAwIDAwLS45OCAxVjloLTIuOTNBMy45OSAzLjk5IDAgMDExNCAzLjc0YzIuMiAwIDQgMS44IDQgNHYyLjE4bC42Mi40NC45LjYzLjkzLS43M2MyLjA5IDAgMy43Ni0xLjMzIDMuNzYtMi45N1Y0LjNsMy44MS0yLjQ2TDE2LjM0LjU2bC0xMi4wOCAxMS44LjY0LjY1IDYuNjQtNi41OXoiLz48L3N2Zz4=',
  
  // Ícone de bloquear usuário
  block: 'data:image/svg+xml;base64,PHN2ZyB4bWxucz0iaHR0cDovL3d3dy53My5vcmcvMjAwMC9zdmciIHZpZXdCb3g9IjAgMCAyNCAyNCIgZmlsbD0iI2YwMCI+PHBhdGggZD0iTTEyIDJDNi40OCAyIDIgNi40OCAyIDEyczQuNDggMTAgMTAgMTAgMTAtNC40OCAxMC0xMFMxNy41MiAyIDEyIDJ6TTQgMTJjMC00LjQyIDMuNTgtOCA4LTggMS44NSAwIDMuNTUuNjMgNC45IDEuNjhMNS42OCAxNi45QTcuOTUgNy45NSAwIDAxNCAxMnptOCA4Yy0xLjg1IDAtMy41NS0uNjMtNC45LTEuNjhMMTguMzIgNy4xQTcuOTUgNy45NSAwIDAxMjAgMTJjMCA0LjQyLTMuNTggOC04IDh6Ii8+PC9zdmc+',
  
  // Ícone de locação (posição no mapa)
  location: 'data:image/svg+xml;base64,PHN2ZyB4bWxucz0iaHR0cDovL3d3dy53My5vcmcvMjAwMC9zdmciIHZpZXdCb3g9IjAgMCAyNCAyNCIgZmlsbD0id2hpdGUiPjxwYXRoIGQ9Ik0xMiAyQzguMTMgMiA1IDUuMTMgNSA5YzAgNS4yNSA3IDE0IDcgMTRzNy04Ljc1IDctMTRjMC0zLjg3LTMuMTMtNy03LTd6bTAgOWMtMS42NiAwLTMtMS4zNC0zLTNzMS4zNC0zIDMtMyAzIDEuMzQgMyAzLTEuMzQgMy0zIDN6Ii8+PC9zdmc+',
  
  // Ícone de editar
  edit: 'data:image/svg+xml;base64,PHN2ZyB4bWxucz0iaHR0cDovL3d3dy53My5vcmcvMjAwMC9zdmciIHZpZXdCb3g9IjAgMCAyNCAyNCIgZmlsbD0id2hpdGUiPjxwYXRoIGQ9Ik0zIDE3LjI1VjIxaDMuNzVMMTcuODEgOS45NGwtMy43NS0zLjc1TDMgMTcuMjV6TTIwLjcxIDcuMDRjLjM5LS4zOS4zOS0xLjAyIDAtMS40MWwtMi4zNC0yLjM0Yy0uMzktLjM5LTEuMDItLjM5LTEuNDEgMGwtMS44MyAxLjgzIDMuNzUgMy43NSAxLjgzLTEuODN6Ii8+PC9zdmc+',
  
  // Função para obter um SVG como string
  getSvg: function(iconName, options = {}) {
    const iconData = this[iconName];
    if (!iconData) {
      console.error(`Ícone não encontrado: ${iconName}`);
      return null;
    }
    
    // Se é um Data URL, extrair SVG
    if (iconData.startsWith('data:image/svg+xml;base64,')) {
      try {
        const base64 = iconData.replace('data:image/svg+xml;base64,', '');
        const decoded = atob(base64);
        
        // Aplicar opções
        if (options.color) {
          return decoded.replace(/fill="[^"]+"/g, `fill="${options.color}"`);
        }
        
        return decoded;
      } catch (error) {
        console.error(`Erro ao decodificar SVG: ${error.message}`);
        return null;
      }
    }
    
    // Se já é um SVG inline
    return iconData;
  },
  
  // Função para obter um elemento SVG
  getSvgElement: function(iconName, options = {}) {
    const svgString = this.getSvg(iconName, options);
    if (!svgString) return null;
    
    const parser = new DOMParser();
    const doc = parser.parseFromString(svgString, 'image/svg+xml');
    const svgElement = doc.documentElement;
    
    // Aplicar tamanho
    if (options.size) {
      svgElement.setAttribute('width', options.size);
      svgElement.setAttribute('height', options.size);
    }
    
    // Aplicar classes
    if (options.className) {
      svgElement.setAttribute('class', options.className);
    }
    
    return svgElement;
  },
  
  // Função para adicionar um ícone a um elemento
  addIconTo: function(element, iconName, options = {}) {
    if (!element) {
      console.error('Elemento não fornecido para addIconTo');
      return null;
    }
    
    const svgElement = this.getSvgElement(iconName, options);
    if (!svgElement) return null;
    
    if (options.clear) {
      element.innerHTML = '';
    }
    
    element.appendChild(svgElement);
    return svgElement;
  }
};

// Exportar como módulo ES ou CommonJS
if (typeof module !== 'undefined' && module.exports) {
  module.exports = VoipIcons;
} else {
  window.VoipIcons = VoipIcons;
}