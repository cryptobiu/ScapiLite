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
	"log"
	"net/http"
	"os"
	"os/exec"
	"path/filepath"

	"github.com/kataras/iris"
)

var (
	urlMatrixBackend = "http://privatepoll.biu-mpc.io/biumatrix/" // BIU backend
	// urlMatrixBackend     = "http://biumatrix.inellipse.com/biumatrix/" // Inellipse backend
	urlMatrix            = "http://35.171.69.162/"
	receavedUserID       = os.Getenv("USER_ID")
	receavedPublicKey    = os.Getenv("PUB_KEY")
	receavedCid          = os.Getenv("CID")
	receavedInstanceID   = os.Getenv("INSTANCE_ID")
	encriptedSimetricKey = ""
	receavedAnswer       = ""
	containerIP          = ""
	containerPort        = ""
	containerMatrixPort  = ""
	pollID               = ""
	logFile              = ""

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
	Id                    string `json:"id"`
	ExternalId            string `json:"externalId"`
	UserId                string `json:"userId"`
	PollID                string `json:"pollId"`
	Answer                string `json:"answer"`
	ContainerIP           string `json:"ip"`
	ContainerPort         string `json:"port"`
	ContainerМatrixPort   string `json:"matrixPort"`
	EncryptedEcryptionKey string `json:"encryptedEcryptionKey"`
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

	fmt.Println("Vesion 0.55")

	app := iris.New()

	// createPublicKeyFile()
	createSimetricFile()
	// decrypt()
	fmt.Println("CID:", os.Getenv("CID"))
	fmt.Println("PUB_KEY:", os.Getenv("PUB_KEY"))
	fmt.Println("USER_ID:", os.Getenv("USER_ID"))
	fmt.Println("INSTANCE_ID:", os.Getenv("INSTANCE_ID"))

	writeFile(receavedPublicKey, "publickey.pub")

	encrypt()
	postSendSimmetricKey()

	app.Post("/poll-answer", func(ctx iris.Context) {
		fmt.Println("POST /poll-answer")
		var data Data
		ctx.ReadJSON(&data)

		receavedAnswer = data.Answer

		writeFile(data.Answer, "inputSalary1.txt")

		ctx.Writef("{\"answer\":\"%s\"}", data.Answer)

		postSendConfirmForRecAnswer()

		fmt.Println("{ answer: ", data.Answer, "}")

	})

	app.Post("/get-poll-params", func(ctx iris.Context) {
		fmt.Println("POST /get-poll-params")
		getContainerInfo()
		var data MatrixResponse
		ctx.ReadJSON(&data)

		url := urlMatrix + "polls/getPollParams/" + pollID + "/" + containerIP + ":" + containerMatrixPort
		fmt.Println("PoolParams URL", url)
		responseJSON := makeGetRequest(url)
		fmt.Println(responseJSON)

		json.Unmarshal([]byte(responseJSON), &data)

		responseForMatrix = data
		fmt.Println("getPollParams", responseForMatrix)
		fmt.Println("PartyID = ", responseForMatrix.PartyID)

		//TODO change hardcoded path with response.CircuitFile
		path := urlMatrix + "polls/circuit/" + data.CircuitFile
		arythmeticFileName = filepath.Base(path)
		DownloadFile(arythmeticFileName, data.CircuitFile)
		DownloadFile("parties.conf", data.PartiesFile)

		fmt.Println("inputSalary1 = ", readFile("inputSalary1.txt"))
		fmt.Println("arythmeticFileName = ", readFile(arythmeticFileName))
		fmt.Println("parties.conf = ", readFile("parties.conf"))

	})

	app.Post("/execute-poll", func(ctx iris.Context) {
		fmt.Println("POST /execute-poll")

		cmd := exec.Command("/bin/bash", "run_protocol.sh",
			responseForMatrix.PartyID,
			responseForMatrix.PartyID,
			responseForMatrix.PartiesNumber,
			receavedAnswer,
			arythmeticFileName,
			"parties.conf",
			responseForMatrix.FieldType,
			responseForMatrix.InternalIterationsNumber,
			responseForMatrix.NG)

		fmt.Println(cmd)

		err := cmd.Run()
		// _, err := cmd.Output()

		if err != nil {
			println("Error!!!", err.Error())
			return
		}

	})

	app.Get("/read-log-file", func(ctx iris.Context) {
		fmt.Println("GET /read-log-file")

		if len(responseForMatrix.PartyID) == 1 {
			logFile = "psmpc_000" + responseForMatrix.PartyID + ".log"
		} else if len(responseForMatrix.PartyID) == 2 {
			logFile = "psmpc_00" + responseForMatrix.PartyID + ".log"
		} else if len(responseForMatrix.PartyID) == 3 {
			logFile = "psmpc_0" + responseForMatrix.PartyID + ".log"
		} else if len(responseForMatrix.PartyID) == 4 {
			logFile = "psmpc_" + responseForMatrix.PartyID + ".log"
		}
		fmt.Println("Logfile = ", logFile)

		ctx.HTML(readFile(logFile))
	})

	app.Get("/parties", func(ctx iris.Context) {
		fmt.Println("GET /parties")
		ctx.HTML(readFile("parties.conf"))
	})

	app.Get("/input", func(ctx iris.Context) {
		fmt.Println("GET /input")
		ctx.HTML(readFile("inputSalary1.txt"))
	})

	app.Get("/output", func(ctx iris.Context) {
		fmt.Println("GET /output")
		ctx.HTML(readFile("output.txt"))
	})

	app.Get("/arythmetic", func(ctx iris.Context) {
		fmt.Println("GET /arythmetic")
		fmt.Print(arythmeticFileName)
		ctx.HTML(readFile(arythmeticFileName))
	})

	app.Get("/container-info", func(ctx iris.Context) {
		response := makeGetRequest(urlMatrixBackend + "offline-instance/" + receavedInstanceID)
		fmt.Println("ContainerInfo: ", response)
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

	data, _ := json.Marshal(qwe)
	fmt.Println("Send JSON to cs")
	fmt.Println(string(data))

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
	var container ContainerInfo

	responseJSON := makeGetRequest(urlMatrixBackend + "offline-instance/" + receavedInstanceID)
	json.Unmarshal([]byte(responseJSON), &container)
	containerIP = container.ContainerIP
	containerPort = container.ContainerPort
	containerMatrixPort = container.ContainerМatrixPort
	pollID = container.PollID
	fmt.Println("containerIP", containerIP)
	fmt.Println("containerPort", containerPort)
	fmt.Println("pollID", pollID)
}
