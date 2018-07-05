package main

import (
	"bytes"
	"crypto/rand"
	"crypto/rsa"
	"crypto/sha256"
	"crypto/x509"
	"encoding/base64"
	"encoding/json"
	"encoding/pem"
	"flag"
	"fmt"
	"io/ioutil"
	"log"
	"net/http"
	"os"
	"os/exec"
	"path/filepath"

	"github.com/kataras/iris"
)

var (
	urlMatrixBackend     = "http://biumatrix.inellipse.com/biumatrix/"
	urlMatrix            = "http://35.171.69.162/"
	receavedUserID       = os.Getenv("USER_ID")
	receavedPublicKey    = os.Getenv("PUB_KEY")
	receavedCid          = os.Getenv("CID")
	receavedInstanceID   = os.Getenv("INSTANCE_ID")
	encriptedSimetricKey = ""
	receavedAnswer       = ""
	containerIP          = ""
	pollID               = ""

	// publicKey = flag.String("public", "files/public.pem", "")
	// privateKey = flag.String("private", "/data/go/rsa-keys-encryption/keys/private.pem", "")

	inputFile     = flag.String("input", "files/01_input.txt", "")
	encryptedFile = flag.String("encrypted", "files/02_encrypted.txt", "")
	decryptedFile = flag.String("decrypted", "files/03_decrypted.txt", "")

	label = flag.String("label", "some random text here", "")

	htmlData = ""

	simmetricKey = ""

	arythmeticFileName = ""
)

type Data struct {
	Answer string `json:"answer"`
}

type ContainerInfo struct {
	ContainerIP string `json:"ip"`
	PollID      string `json:"pollId"`
}

type MatrixResponse struct {
	PartyID                  string `json:"partyID"`
	PartiesNumber            string `json:"partiesNumber"`
	InputFile                string `json:"inputFile"`
	OutputFile               string `json:"outputFile"`
	CircuitFile              string `json:"circuitFile"`
	ProxyAddress             string `json:"proxyAddress"`
	PartiesFile              string `json:"partiesFile"`
	FieldType                string `json:"fieldType"`
	InternalIterationsNumber string `json:"internalIterationsNumber"`
	NG                       string `json:"NG"`
}

var responseForMatrix MatrixResponse

func main() {

	app := iris.New()

	// createPublicKeyFile()
	createSimetricFile()
	// decrypt()
	fmt.Println("CID:", os.Getenv("CID"))
	fmt.Println("PUB_KEY:", os.Getenv("PUB_KEY"))
	fmt.Println("USER_ID:", os.Getenv("USER_ID"))
	fmt.Println("INSTANCE_ID:", os.Getenv("INSTANCE_ID"))

	encrypt()
	postSendSimmetricKey()

	//Method GET: http://localhost:8080/read
	app.Get("/read", func(ctx iris.Context) {
		inputTxt, err := ioutil.ReadFile("files/01_input.txt")
		encryptedTxt, err := ioutil.ReadFile("files/02_encrypted.txt")

		check(err)
		fmt.Println("------------------")
		fmt.Print(string(inputTxt))
		fmt.Print(string(encryptedTxt))
		fmt.Println(htmlData)
		ctx.HTML(string(inputTxt))
		ctx.HTML(string("<br>"))
		ctx.HTML(string(encryptedTxt))
		ctx.HTML(string("<br>"))
		ctx.HTML(htmlData)
	})

	// Method POST: http://localhost:8080/poll-answer
	app.Post("/poll-answer", func(ctx iris.Context) {
		var data Data
		ctx.ReadJSON(&data)
		// ctx.Writef("Answer %s userID %s", data.Answer, data.userID)
		// receavedValues := Data{data.Answer}
		// ctx.Writef(data.Answer)
		receavedAnswer = data.Answer
		writeFile(data.Answer, "inputSalary1.txt")
		ctx.Writef("{\"answer\":\"%s\"}", data.Answer)

		postSendConfirmForRecAnswer()

		// answer := data.Answer
		// userID := data.userID

		fmt.Println("{ answer: ", data.Answer, "}")

		// fmt.Println(userID)

	})

	app.Post("/get-poll-params", func(ctx iris.Context) {
		getContainerInfo()
		var data MatrixResponse
		ctx.ReadJSON(&data)

		url := urlMatrix + "polls/getPollParams/" + pollID + "/" + containerIP
		fmt.Println("PoolParams URL", url)
		// url := "http://​35.171.69.162​/polls/getPollParams/Test/172.18.28.135"
		responseJSON := makeGetRequest(url)
		fmt.Println(responseJSON)

		json.Unmarshal([]byte(responseJSON), &data)

		responseForMatrix = data
		fmt.Println("getPollParams", responseForMatrix)

		//TODO change hardcoded path with response.CircuitFile
		path := urlMatrix + "polls/circuit/" + data.CircuitFile
		arythmeticFileName = filepath.Base(path)
		fmt.Println(arythmeticFileName)
		DownloadFile(arythmeticFileName, data.CircuitFile)
		DownloadFile("parties.conf", data.PartiesFile)

	})

	app.Post("/execute-poll", func(ctx iris.Context) {

		// PartyID := "0"
		// PartiesNumber := "3"
		// InputFile := "inputSalary0.txt"
		// OutputFile := "output.txt"
		// CircuitFile := "ArythmeticVarianceFor3InputsAnd0Parties.txt"
		// // ProxyAddress := response.ProxyAddress
		// FieldType := "GF2_8LookupTable"
		// InternalIterationsNumber := "1"
		// ng := "1"
		// PartiesFile := "partiesNG.conf"

		// cmd := exec.Command("./HyperMPC", "-partyID", PartyID, "-partiesNumber", PartiesNumber, "-inputFile", InputFile, "-outputFile", OutputFile, "-circuitFile", CircuitFile, "-partiesFile", PartiesFile, "-fieldType", FieldType, "-internalIterationsNumber", InternalIterationsNumber, "-NG", ng)
		// cmd := exec.Command("/bin/bash", "hypermpc/script.sh")

		cmd := exec.Command("/bin/bash", "run_protocol.sh", responseForMatrix.PartyID, responseForMatrix.PartyID, responseForMatrix.PartiesNumber, responseForMatrix.InputFile, arythmeticFileName, "parties.conf", responseForMatrix.FieldType, responseForMatrix.InternalIterationsNumber, responseForMatrix.NG)

		fmt.Println(cmd)

		out, err := cmd.Output()

		if err != nil {
			println("Error!!!", err.Error())
			return
		}

		fmt.Println("from out", string(out))

	})
	app.Get("/read-log-file", func(ctx iris.Context) {

		ctx.HTML(readFile("psmpc_0001.log"))
	})

	app.Run(iris.Addr(":8080"), iris.WithCharset("UTF-8"))
}

func createSimetricFile() {
	simmetricKey = generateRandomString()
}

func encrypt() {
	// inputTxt, err := ioutil.ReadFile("files/01_input.txt")
	rootPEM := receavedPublicKey

	block, _ := pem.Decode([]byte(rootPEM))

	pub, err := x509.ParsePKCS1PublicKey(block.Bytes)
	if err != nil {
		log.Fatalf("bad public key: %s", err)
	}

	secretMessage := []byte(simmetricKey)
	label := []byte("")

	rng := rand.Reader

	ciphertext, err := rsa.EncryptOAEP(sha256.New(), rng, pub, secretMessage, label)
	if err != nil {
		fmt.Fprintf(os.Stderr, "Error from encryption: %s\n", err)
		return
	}

	// Since encryption is a randomized function, ciphertext will be
	// different each time.

	encriptedSimetricKey = base64.StdEncoding.EncodeToString(ciphertext)
}

func postSendSimmetricKey() {
	// url := urlMatrixBackend + "offline-instance"
	url := urlMatrixBackend + "cs"
	fmt.Println("postSendSimmetricKey URL:", url)

	cidJSON := "cid"
	encKeyJSON := "encKey"
	qwe := make(map[string]string)
	qwe[cidJSON] = receavedCid
	qwe[encKeyJSON] = encriptedSimetricKey
	//we send simmetricPlainKey insted of encrypted and encoded with base64
	// qwe[encKeyJSON] = simmetricPlainKey

	data, _ := json.Marshal(qwe)
	fmt.Println("Send JSON to cs")
	fmt.Println(string(data))

	// var jsonStr = []byte(`{"cid":"cid", "encKey":"encriptedSimetricKey"}`)
	var jsonStr = []byte(string(data))
	req, err := http.NewRequest("POST", url, bytes.NewBuffer(jsonStr))
	req.Header.Set("Content-Type", "application/json")

	client := &http.Client{}
	resp, err := client.Do(req)
	if err != nil {
		panic(err)
	}

	defer resp.Body.Close()

	fmt.Println("Response for postSendSimmetricKey \n", resp.Status)
	htmlData = string(resp.Status)

}

func postSendConfirmForRecAnswer() {
	// url := urlMatrixBackend + "biumatrix/cs"
	url := urlMatrixBackend + "offline-instance"

	cidJSON := "cId"
	answerJSON := "answer"
	qwe := make(map[string]string)
	qwe[cidJSON] = receavedCid
	qwe[answerJSON] = "true"

	data, _ := json.Marshal(qwe)
	fmt.Println("Send JSON to offline-instance")
	fmt.Println(string(data))

	// var jsonStr = []byte(`{"cid":"cid", "encKey":"encriptedSimetricKey"}`)
	var jsonStr = []byte(string(data))
	req, err := http.NewRequest("PUT", url, bytes.NewBuffer(jsonStr))
	req.Header.Set("Content-Type", "application/json")

	client := &http.Client{}
	resp, err := client.Do(req)
	if err != nil {
		panic(err)
	}

	defer resp.Body.Close()

	fmt.Println("Response for postSendConfirmForRecAnswer \n", resp.Status)
	htmlData = string(resp.Status)

}

func getContainerInfo() {
	var data ContainerInfo
	responseJSON := makeGetRequest(urlMatrixBackend + "offline-instance/" + receavedInstanceID)
	json.Unmarshal([]byte(responseJSON), &data)
	containerIP = data.ContainerIP
	pollID = data.PollID
	fmt.Println("containerIP", containerIP)
	fmt.Println("pollID", pollID)
}

// func decrypt() {

// 	flag.Parse()

// 	// Read the input file
// 	in, err := ioutil.ReadFile(*encryptedFile)
// 	if err != nil {
// 		log.Fatalf("input file: %s", err)
// 	}

// 	// Read the private key
// 	pemData, err := ioutil.ReadFile(*privateKey)
// 	if err != nil {
// 		log.Fatalf("read key file: %s", err)
// 	}

// 	// Extract the PEM-encoded data block
// 	block, _ := pem.Decode(pemData)
// 	if block == nil {
// 		log.Fatalf("bad key data: %s", "not PEM-encoded")
// 	}

// 	// Decode the RSA private key
// 	priv, err := x509.ParsePKCS1PrivateKey(block.Bytes)
// 	if err != nil {
// 		log.Fatalf("bad private key: %s", err)
// 	}

// 	// Decrypt the data
// 	out, err := rsa.DecryptOAEP(sha1.New(), rand.Reader, priv, in, []byte(*label))
// 	if err != nil {
// 		log.Fatal(err)
// 	}

// 	// Write data to output file
// 	if err := ioutil.WriteFile(*decryptedFile, out, 0600); err != nil {
// 		log.Fatalf("write output: %s", err)
// 	}

// }

// use generated keys from java
// func generateKeys() {
// 	reader := rand.Reader
// 	bitSize := 2048

// 	key, err := rsa.GenerateKey(reader, bitSize)
// 	checkError(err)

// 	publicKey := key.PublicKey

// 	saveGobKey("private.key", key)
// 	savePEMKey("private.pem", key)

// 	saveGobKey("public.key", publicKey)
// 	savePublicPEMKey("public.pem", publicKey)
// }
