package main

import (
	"log"
	"net/http"
)

var gldata = make(map[string]string)

func store(w http.ResponseWriter, req *http.Request) {
	log.Println("Received store request.")
	key := req.Header.Get("key")

	if key == "" {
		w.Write([]byte("Key not found"))
		return
	}

	value := req.Header.Get("value")
	if value == "" {
		w.Write([]byte("Value not found"))
		return
	}

	gldata[key] = value

	w.Write([]byte("Stored " + key + " -> " + value))
}

func retrieve(w http.ResponseWriter, req *http.Request) {
	log.Println("Received retrieve request.")
	key := req.Header.Get("key")

	if key == "" {
		w.Write([]byte("Key not found"))
		return
	}

	if value, ok := gldata[key]; ok {
		w.Write([]byte(value))
	} else {
		w.Write([]byte("No data found for key: " + key))
	}
}

func listAll(w http.ResponseWriter, req *http.Request) {
	log.Println("Received list request.")
	result := ""

	for k, v := range gldata {
		result += k + "->" + v + "\n"
	}
	w.Write([]byte(result))
}

func main() {
	http.HandleFunc("/store", store)
	http.HandleFunc("/retrieve", retrieve)
	http.HandleFunc("/list", listAll)
	log.Println("Starting server...")
	http.ListenAndServe(":8080", nil)
}
