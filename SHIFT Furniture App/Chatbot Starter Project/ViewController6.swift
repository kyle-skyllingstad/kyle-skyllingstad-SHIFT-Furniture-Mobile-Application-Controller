//
//  ViewController6.swift
//  SHIFT Furniture MOBILE APPLICATION
//  Created by Kyle Skyllingstad
//

// Import necessary kits/libraries
import UIKit
import ApiAI
import AVFoundation
import Speech

// Class for screen 6, ViewController6
 class ViewController6: UIViewController, SFSpeechRecognizerDelegate, UITextFieldDelegate {
    
    
    // Outlets set for entities
    
    // Set button for microphone on/off
    @IBOutlet weak var microphoneButton: UIButton!
    
    // Invisible IP Address output Label set for storage purposes
    @IBOutlet var IPALabel: UILabel!
    
    // A.I. Chatbot text field and label
    @IBOutlet weak var InstructionField: UITextField!
    @IBOutlet weak var DialogflowResponse: UILabel!
    
    
    
    // Variable initialization
    
    // Create speech recognition entity
    private let speechRecognizer = SFSpeechRecognizer(locale: Locale.init(identifier: "en-US"))
    
    
    // Initialize speech recognition variables as optionals
    private var recognitionRequest: SFSpeechAudioBufferRecognitionRequest?
    private var recognitionTask: SFSpeechRecognitionTask?
    private let audioEngine = AVAudioEngine()
    
    
    
    // Main screen loading function
    override func viewDidLoad() {
        super.viewDidLoad()
        
        // Set InstructionField text field to delegate mode to take in speech and text
        self.InstructionField.delegate = self
        
        
        
        
        // Some speech recognition begins here
        
        // Make microphone button off
        microphoneButton?.isEnabled = false
        
        
        // Set speech recognizer
        speechRecognizer?.delegate = self
        
        // Requesting speech recognizer
        SFSpeechRecognizer.requestAuthorization { (authStatus) in
            
            // Turn button inactive
            var isButtonEnabled = false
            
            switch authStatus {
                
            // If function is authorized, turn the button to active
            case .authorized:
                    isButtonEnabled = true
            
            // If function is unauthorized, turn the button back to inactive
            case .denied:
                isButtonEnabled = false
                print("User denied access to speech recognition")
             
            // If function use is restricted, turn the button back to inactive
            case .restricted:
                isButtonEnabled = false
                print("Speech recognition restricted on this device")
             
            // If function authorization is indeterminate, turn the button back to inactive
            case .notDetermined:
                isButtonEnabled = false
                print("Speech recognition not yet authorized")
            }
                
            // Set the new state order of the microphone button to next in line of functions to be carried out
            OperationQueue.main.addOperation() {
                
                // Command the button to be in its ordered state (active or inactive)
                self.microphoneButton?.isEnabled = isButtonEnabled
            }
        }
                
        // End of this first speech recognition section
        
        
        
        
        
        
        // IP Address intake section
        
        // Call reception notification center to receive incoming information (text)
        NotificationCenter.default.addObserver(self, selector: #selector(didGetNotificationIPA(_:)), name: Notification.Name("IPAin"), object: nil)
        
        // End of IP Address intake section
        
    }
    
    
    
    // Hide keyboard when the user touches outside the keyboard area
    override func touchesBegan(_ touches: Set<UITouch>, with event: UIEvent?) {
        self.view.endEditing(true)
    }
    
    // Hide keyboard when the user presses the return key
    func textFieldShouldReturn(_ textField: UITextField) -> Bool {
        InstructionField.resignFirstResponder()
        return (true)
    }
    
    
    
    
    
    // More speech recognition background in this section
    
    // When begin/stop recording speech button is tapped
    @IBAction func microphoneTapped(_ sender: AnyObject) {
        
        // When microphone button "Off" is tapped while recording
        // If the audio recording is happening
        if audioEngine.isRunning {
            
            // Tell it to stop recording
            audioEngine.stop()
            recognitionRequest?.endAudio()
            
            // Inactivate microphone button
            microphoneButton?.isEnabled = false
            
            // Set microphone button text to "Start Recording"
            microphoneButton?.setTitle("Start Recording", for: .normal)
            
        // If microphone button "On" tapped while not recording
        } else {
            
            // Now start listening and recording to speech
            startRecording()
            
            // Change microphone button text to "Stop Recording"
            microphoneButton?.setTitle("Stop Recording", for: .normal)
        }
    }
    
    
    
    
    
    
    // Functions ////////////////////////////////
    
    
    
    // Function called to start the recording of spoken words and convert this into written text
    func startRecording() {
        
        // Make sure the current state of the speech recognition task is idle; that is, if a speech recognition task is already ongoing from before, it must be ended for a new one to be executed when this function is newly called.
        if recognitionTask != nil {
            recognitionTask?.cancel()
            recognitionTask = nil
        }
        
        // Old code for microphone testing/modification purposes
        /*
        let audioSession = AVAudioSession.sharedInstance()
        do {
            try audioSession.setCategory(AVAudioSessionCategoryRecord)
            try audioSession.setMode(AVAudioSessionModeMeasurement)
            try audioSession.setActive(true, with: .notifyOthersOnDeactivation)
        } catch {
            print("audioSession properties weren't set because of an error.")
        }
        */
        
        
        
        
        // Request buffer is finished
        recognitionRequest = SFSpeechAudioBufferRecognitionRequest()
        
        // Set speech engine input node
        let inputNode = audioEngine.inputNode 
        
        // Create a speech recognition request
        guard let recognitionRequest = recognitionRequest else {
            fatalError("Unable to create an SFSpeechAudioBufferRecognitionRequest object")
        }
        
        // Allow a partial (incomplete) speech request to still be processed
        recognitionRequest.shouldReportPartialResults = true

        
        // Create speech recognition task
        recognitionTask = speechRecognizer?.recognitionTask(with: recognitionRequest, resultHandler: { (result, error) in
            
            // Set the final counter variable to false for default
            var isFinal = false
            
            // If there is some inputted information
            if result != nil {
                
                // Process the most accurate transcription of the inputted speech and display it in the insctruction UI text field
                self.InstructionField.text = result?.bestTranscription.formattedString
                
                // Set recording state to final
                isFinal = (result?.isFinal)!
            }
            
            // If there is an error during the speaking process or the recording state is set to final
            if error != nil || isFinal {
                
                // Stop recording process
                self.audioEngine.stop()
                inputNode.removeTap(onBus: 0)
                
                // Set the recognition request and task to empty (void)
                self.recognitionRequest = nil
                self.recognitionTask = nil
                
                // Activate the microphone button
                self.microphoneButton?.isEnabled = true
            }
        })
        
        
        // Format the recording
        let recordingFormat = inputNode.outputFormat(forBus: 0)
        inputNode.installTap(onBus: 0, bufferSize: 1024, format: recordingFormat) { (buffer, when) in
            self.recognitionRequest?.append(buffer)
        }
        
        // Set audio recording function for the next speech iteration
        audioEngine.prepare()
        
        do {
            
            // Test the audio recording function
            try audioEngine.start()
        } catch {
            print("audioEngine couldn't start because of an error.")
        }
        
        // Old code for testing purposes
        // textView.text = "Say something, I'm listening!"
        
        // Set the instruction UI text field empty for now
        InstructionField.text = ""
        
    }
    
    
    
    

    // Function that is called to identify when speech is spoken
    func speechRecognizer(_ speechRecognizer: SFSpeechRecognizer, availabilityDidChange available: Bool) {
        if available {
            
            // Enable the microphone button if device is ready to listen or not listen
            microphoneButton.isEnabled = true
        } else {
            
            // Disable the microphone button if the device is in transition between states
            microphoneButton.isEnabled = false
        }
    }
    
    // End of this additional speech recognition stuff
    
    
    
  
    
    
    
    
    // AI chatbot Section ///////////////////////
    
    
    
    // Function for if IP Address is received by this screen (VC6)
    @objc func didGetNotificationIPA(_ notification: Notification) {
        
        // Read in raw notification
        let IPAinOptional = notification.object as! String?
        // Unwrap raw optional
        guard let IPAin: String = IPAinOptional as? String else {
            return
        }
        
        // Set IP Address label to the IP Address
        IPALabel.text = "IPA: \(IPAin)"
    }
    
    
    
    
    

    
    
    // Action if "Send Message" is tapped
    @IBAction func SendInstructions(_ sender: Any) {
        
        // Initialize IP Address string
        let HttpString: String = "http://"

        // Old code for testing purposes
        // Where a hardcoded IP Address could be set
        // let IPAString = "esp8266.local"
        
        
        
        // Convert label Optional String to String, then parse to just the Location Access Token with nothing before it
        
        // Read in IPA optional from label text
        let IPAStringOptional = IPALabel.text
        
        // Unwrap optional string
        guard let IPAStringRaw: String = IPAStringOptional as? String else {
            return
        }
        
        // Prevent the label from displaying "IPA" twice in the line - again here we want just the numeric IP Address string with no words before it.
        let IPAString = IPAStringRaw.replacingOccurrences(of: "IPA: ", with: "")
        
  
        
        
        
        // Create variable for and get the text request from the user's inputted query.
         let request = ApiAI.shared().textRequest()
         
         
         // Get the text from the message field, or UI Text Field.
         // If while text is in message field (it is not empty), the query property of the request equals the text.
         if let text = self.InstructionField.text, text != "" {
            
             // If so, set the query property of the request equal to the inputted text.
             request?.query = text
         } else {
             return
         }
         
         // When the request is completed, call the setMappedCompletionBlockSuccess method to say what will happen upon the closure.
         request?.setMappedCompletionBlockSuccess({ (request, response) in
             
             // Create and store response variable
             let response = response as! AIResponse
             
             // if while the app is receiving a valid, successful response from the dialogflow agent, we record its response to later be shown to the user.
             if let textResponse = response.result.fulfillment.speech {
                 
             
                 // call speechAndText have the text response spoken aloud to the user, and for the text response to be sent to the UI label object.
                 self.speechAndText(text: textResponse)
                 // print(textResponse) // Test code
                 // print(textResponse) // Test code
                 

                // Convert Dialogflow AI response to just numbers
                let NumberString = textResponse.components(separatedBy: CharacterSet .decimalDigits.inverted).joined()
                
                // Convert to double for analysis and computational adjustments (e.g. unit changes)
                var NumberDouble = Double(NumberString)
                
                
                // No number output because Dialogflow does not accept command as fully specified
                if NumberDouble == nil && textResponse.contains("stop") == false {
                    print("Error: furniture, direction, and/or distance not fully specified")
                }
                    
                // Command accepted as fully specified in Dialogflow
                else {
                    
                    // Convert all units to mm
                    var marker = 0
                    if textResponse.contains(" cm.") {
                        // Convert to mm
                        NumberDouble = NumberDouble!*10
                    }
                    else if textResponse.contains(" dm.") {
                        // Convert to mm
                        NumberDouble = NumberDouble!*100
                    }
                    else if textResponse.contains(" m.") {
                        // Convert to mm
                        NumberDouble = NumberDouble!*1000
                    }
                    else if textResponse.contains(" inch.") {
                        // Convert to mm
                        NumberDouble = NumberDouble!*25.4
                    }
                    else if textResponse.contains(" ft.") {
                        // Convert to mm
                        NumberDouble = NumberDouble!*304.8
                    }
                    
                    
                    if textResponse.contains("stop") == true {
                        NumberDouble = 0
                    }
                    
                    // Turn double units to integers
                    let NumberInteger = Int(NumberDouble!)
                    
                    // Put all integers into strings with exactly five units tso they can be stacked and sent to the Arduino master program for controlling the SHIFT Pods.
                    let NumberString: String = String(format: "%04d", NumberInteger)
                    
                    

                
                
                    
                    
                    // Get the directional string, or action mode, from the text response of the chatbot.
                    let DirectionString = self.GetDirectionString(textin: textResponse)
                    
                    // Put everything into a single string to be sent to the Arduino code through a WiFi URL
                    let urlString = "\(HttpString)\(IPAString)\(DirectionString)\(NumberString)"
                    print(urlString)
                    
                    // Begin URL session
                    let session = URLSession.shared
                    
                    // Convert it all to an actual URL
                    // let url = URL(string: urlString)!
                    guard let url = URL(string: urlString)
                        else {
                            return
                    }
                    
                    // Send request to the URL without actually opening it in a web browser
                    let task = session.dataTask(with: url, completionHandler: { data, response, error in
                    })
                    task.resume()
                    
                }

                 
             }
              
            
         // If the app does not get any sort of response from the dialogflow agent (NOT including "Sorry, I didn't get that, etc.)
         }, failure: { (request, error) in
             
             // Print out the error in the Xcode console
             print(error!)
         })
         
         // Initiate the request the app is sending to the AI, or API.AI
         // Call the enqeue method to do so, specifying the request properly.
         ApiAI.shared().enqueue(request)
         
         // Clear the text present in the UI text field.
        InstructionField.text = ""
        
    }

    
    
    
    
    
    
    // Function that turns Dialogflow AI direction response, or action mode, into an output string showing direction, or two directions if furniture told to be moved sideways (turn, then move straight forward)
    func GetDirectionString(textin: String) -> String {
        
        // Initialize the direction string, or action mode string
        var DirectionString = "0"
        
        
        // If chatbot response response contains "forward"
        if textin.contains("forward") {
            // Action mode 1
            var DirectionString: String = "/get?input1=1"
            return DirectionString
        }
            
            
        // If chatbot response response contains "backward"
        else if textin.contains("backward") {
            // Action mode 2
            var DirectionString: String = "/get?input1=2"
            return DirectionString
        }
            
            
        // If chatbot response response contains "to the right"
        else if textin.contains("to the right") {
            
            // If chatbot response contains just an angle, which would mean it would contain "degrees"
            if textin.contains("degrees") {
                // Action mode 4
                var DirectionString: String = "/get?input1=4"
                return DirectionString
            }
                
            // Otherwise it is a request to change direction and move, i.e. "Move the couch 3 feet to the right."
            else {
                // Turn 90 degrees to the right (4) then put in foward position (1)
                var DirectionString: String = "/get?input1=400901"
                return DirectionString
            }
        }
            
            
            
        // If chatbot response response contains "to the left"
        else if textin.contains("to the left") {
            
            // If chatbot response contains just an angle, which would mean it would contain "degrees"
            if textin.contains("degrees") {
                // Action mode 3
                var DirectionString: String = "/get?input1=3"
                return DirectionString
            }
                
                
            // Otherwise it is a request to change direction and move, i.e. "Move the couch 3 feet to the left."
            else {
                // Turn 90 degrees to the left (3) then put in foward position (1)
                var DirectionString: String = "/get?input1=300901"
                return DirectionString
            }
        }
         
            
            
        // If chatbot response response contains "stop"
        else if textin.contains("stop") {
            // Action mode 5
            var DirectionString: String = "/get?input1=5"
            return DirectionString
        }
        return DirectionString
    }
    
    
    
    
    // Function for if back button from this screen (VC6) back to the previous one (VC5) is tapped
    @IBAction func didTapButton6to5() {
        
        // Get IP Address from invisible storage label
        let IPAStringOptional = IPALabel.text
        
        // Unwrap optional
        guard let IPAStringRaw: String = IPAStringOptional as? String else {
            return
        }
        
        // Prevent the label from displaying "IPA" twice in the line and storing the wrong data
        let IPAString = IPAStringRaw.replacingOccurrences(of: "IPA: ", with: "")
        
        
        
        // Create link to VC5
        guard let VC = storyboard?.instantiateViewController(identifier: "VC5") as? ViewController5 else {
            return
        }
        
        // Present (go to) VC5
        VC.modalPresentationStyle = .fullScreen
        present(VC, animated: true)
        
        
        
        // Post notification of inputted IP Address for viewing on this screen and others
        NotificationCenter.default.post(name: Notification.Name("IPAin"), object: IPAString)
          
    }
    
    
    
    
    
    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
        // Dispose of any resources that can be recreated to save memory and file space.
    }
    
    
    
    // Create speech synthesizer instance
    let speechSynthesizer = AVSpeechSynthesizer()
     
    // Function that returns speech and text for the Dialogflow AI response when called
    func speechAndText(text: String) {
        
        // Get the AI speech utterance response
        let speechUtterance = AVSpeechUtterance(string: text)
        
        // Actually carry out the spoken AI response
        speechSynthesizer.speak(speechUtterance)
        
        // Animate the visual AI response
        UIView.animate(withDuration: 1.0, delay: 0.0, options: .curveEaseInOut, animations: {
            self.DialogflowResponse.text = text
        }, completion: nil)
    }
    


}
