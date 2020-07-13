//
//  ViewController5.swift
//  SHIFT Furniture MOBILE APPLICATION
//  Created by Kyle Skyllingstad
//

// Import necessary kits
import UIKit

// Class for screen 5, ViewController5
class ViewController5: UIViewController {
    
    // Outlets set for entities
    
    // Invisible IP Address label for storing it
    @IBOutlet var IPALabel: UILabel!

    
    // Main screen loading function
    override func viewDidLoad() {
        super.viewDidLoad()

        // Call notification center function to take in the IP Address posted in the previous screen (VC4)
        NotificationCenter.default.addObserver(self, selector: #selector(didGetNotificationIPA(_:)), name: Notification.Name("IPAin"), object: nil)
    }
    
    
    
    
    
    
    // Functions ////////////////////////////////
    
    
    
    // Function for if IP Address is received
    @objc func didGetNotificationIPA(_ notification: Notification) {
        
        // Read in raw notification
        let IPAinOptional = notification.object as! String?
        
        // Unwrap raw optional
        guard let IPAin: String = IPAinOptional as? String else {
            return
        }
        
        // Set IP Address Label text to the IP Address
        IPALabel.text = "IPA: \(IPAin)"
    }
    
    
    
    
    
    // Function for if button from this screen (VC5) to the next screen (VC6) tapped
    @IBAction func didTapButton5to6() {
        
        // Tread in raw IP Address label string from the storage label
        let IPAStringOptional = IPALabel.text
        
        // Unwrap raw optional
        guard let IPAStringRaw: String = IPAStringOptional as? String else {
            return
        }
        
        // Prevent the label from displaying "IPA" twice in the line - just capture the numeric IP Address string
        let IPAString = IPAStringRaw.replacingOccurrences(of: "IPA: ", with: "")
 
        
        // Create link to the next screen (VC6)
        guard let VC = storyboard?.instantiateViewController(identifier: "VC6") as? ViewController6 else {
            return
        }
        
        // Present (go to) VC6
        VC.modalPresentationStyle = .fullScreen
        present(VC, animated: true)
        
        
        // Post notification of IP Address for viewing on this screen and others
        NotificationCenter.default.post(name: Notification.Name("IPAin"), object: IPAString)
            
    }
    
    
    
    
    // Function for if back button from this screen (VC4.5) back to the previous one (VC4) tapped
    @IBAction func didTapButton5to4() {
        
        // Get IP Address from invisible storage label
        let IPAStringOptional = IPALabel.text
        
        // Unwrap optional
        guard let IPAStringRaw: String = IPAStringOptional as? String else {
            return
        }
        
        // Prevent the label from displaying "IPA" twice in the line and storing the wrong data
        let IPAString = IPAStringRaw.replacingOccurrences(of: "IPA: ", with: "")
        
        
        
        // Create link to VC4
        guard let VC = storyboard?.instantiateViewController(identifier: "VC4") as? ViewController4 else {
            return
        }
        
        // Present (go to) VC4
        VC.modalPresentationStyle = .fullScreen
        present(VC, animated: true)
        
        
        
        // Post notification of inputted IP Address for viewing
        NotificationCenter.default.post(name: Notification.Name("IPAin"), object: IPAString)
          
    }
    



}
