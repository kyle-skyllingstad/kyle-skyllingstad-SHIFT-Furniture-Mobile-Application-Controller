//
//  ViewController3.swift
//  SHIFT Furniture MOBILE APPLICATION
//  Created by Kyle Skyllingstad
//

// Import necessary kits
import UIKit

// Class for screen 3, ViewController3
class ViewController3: UIViewController, UITextFieldDelegate {
    
    // Text fields set
    // Text field where IP Address is to be entered
    @IBOutlet var IPAField: UITextField!
    
    // Output Labels set
    // Label to display IP Address when saved
    @IBOutlet weak var IPALabel: UILabel!

    
    // Main screen loading function
    override func viewDidLoad() {
        super.viewDidLoad()
        
        
        
        
        // Set up the IP Address field to allow the keyboard to be able to be hidden after typing has finished
        self.IPAField.delegate = self
        

        
        
        // Call reception notification center function to receive incoming information (text that contains the IP Address)
        NotificationCenter.default.addObserver(self, selector: #selector(didGetNotificationIPA(_:)), name: Notification.Name("IPAin"), object: nil)
        
    }
    
    
    
    // Hide keyboard when the user touches outside the keyboard
    override func touchesBegan(_ touches: Set<UITouch>, with event: UIEvent?) {
        self.view.endEditing(true)
    }
    
    // Hide keyboard when the user presses the return key
    func textFieldShouldReturn(_ textField: UITextField) -> Bool {
        
        IPAField.resignFirstResponder()
        return (true)
    }
    
    
    
    
    
    // Function for if IP Address Button tapped
    @IBAction func didTapSaveIPA() {
        
        // Post notification of inputted IP Address text to be available to this screen, VC3, as well as the other screens (ViewControllers)
        NotificationCenter.default.post(name: Notification.Name("IPAin"), object: IPAField.text)
    }
    
    
    
    
    
    
    // Functions ////////////////////////////////
    
    
    
    // Function for If IP Address is received
    @objc func didGetNotificationIPA(_ notification: Notification) {
        
        // Take in raw notification
        let IPAinOptional = notification.object as! String?
        
        // Unwrap optional raw notification 
        guard let IPAin: String = IPAinOptional as? String else {
            return
        }
        
        // Set the IP Address label on this screen equal to the in-house received IP Address string - to show the user the IP Address has been saved as shown and reaffirm it for them
        IPALabel.text = "IP Address: \(IPAin)"
    }
    
    
    
    
    
    
    // Function for if button from this screen (VC3) to screen 4 (VC4) tapped
    @IBAction func didTapButton3to4() {
        
        // Convert label Optional String to String, then parse to just the Location Access Token with nothing before it
        let IPAStringOptional = IPALabel.text
        
        // Unwrap optional
        guard let IPAStringRaw: String = IPAStringOptional as? String else {
            return
        }
        
        // Concatenate the IP Address string to just show the IP Address and not any identifier words before it, since it is supposed to be only the IP Address and not the raw text from the label which would include the words, "IP Address" before the actual numeric string IP Address characters.
        let IPAString = IPAStringRaw.replacingOccurrences(of: "IP Address: ", with: "")
        
        

        
        
        // Create link to screen 4, or VC4
        guard let VC = storyboard?.instantiateViewController(identifier: "VC4") as? ViewController4 else {
            return
        }
        
        // Present (go to) VC4
        VC.modalPresentationStyle = .fullScreen
        present(VC, animated: true)
        
        
        
        
        // Post notification of inputted IP Address string for other screens to view downstream. They will call this object in their screens' ViewController classes.
        NotificationCenter.default.post(name: Notification.Name("IPAin"), object: IPAString)
            
    }
    
    
    
    
    // Function for if back button from this screen (VC3) back to screen 2 (VC2) tapped
    @IBAction func didTapButton3to2() {
        
        // Create link back to VC2
        guard let VC = storyboard?.instantiateViewController(identifier: "VC2") as? ViewController2 else {
            return
        }
        
        // Present (go to) VC2
        VC.modalPresentationStyle = .fullScreen
        present(VC, animated: true)
    }

    
    

}
