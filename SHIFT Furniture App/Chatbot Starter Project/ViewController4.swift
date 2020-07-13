//
//  ViewController4.swift
//  SHIFT Furniture MOBILE APPLICATION
//  Created by Kyle Skyllingstad
//

// Import necessary kits
import UIKit

// Class for screen 4, ViewController4
class ViewController4: UIViewController {
    
    // Outlets set for entities
    
    // IP Address label (not to be shown, only to store the incoming IP Address from the previous screen, VC3)
    @IBOutlet var IPALabel: UILabel!

    
    // Main screen loading function
    override func viewDidLoad() {
        super.viewDidLoad()
        
        // Call reception notification center function to receive incoming information (text that contains the IP Address)
        NotificationCenter.default.addObserver(self, selector: #selector(didGetNotificationIPA(_:)), name: Notification.Name("IPAin"), object: nil)
        
    }
    
    
    
    
    
    
    // Functions ////////////////////////////////
    
    
    
    // Function for if IP Address received
    @objc func didGetNotificationIPA(_ notification: Notification) {
        
        // Take in raw IP Address notification string
        let IPAinOptional = notification.object as! String?
        
        // Unwrap optional raw notification
        guard let IPAin: String = IPAinOptional as? String else {
            return
        }
        
        // Set invisible IP Address label on this screen equal to the incoming IP Address for storage purposes
        IPALabel.text = "IPA: \(IPAin)"
    }
    
    
    
    
    
    // Function for if button from this screen (VC4) to the next screen, VC 5, is tapped
    @IBAction func didTapButton4to5() {
        
        // Get IP Address from invisible storage label
        let IPAStringOptional = IPALabel.text
        
        // Unwrap optional
        guard let IPAStringRaw: String = IPAStringOptional as? String else {
            return
        }
        
        // Prevent the label from displaying "IPA" twice in the line and storing the wrong data
        let IPAString = IPAStringRaw.replacingOccurrences(of: "IPA: ", with: "")
        
        
        // Create link to the next scren, VC5
        guard let VC = storyboard?.instantiateViewController(identifier: "VC5") as? ViewController5 else {
            return
        }
        
        // Present (go to) VC5
        VC.modalPresentationStyle = .fullScreen
        present(VC, animated: true)
        
        
        
        // Post notification of inputted IP Address for this screen and others to view
        NotificationCenter.default.post(name: Notification.Name("IPAin"), object: IPAString)
            
    }
    
    
    
    
    
    // Function fo if back button from this screen (VC4) back to VC3 tapped
    @IBAction func didTapButton4to3() {
        
        // Get IP Address from invisible storage label
        let IPAStringOptional = IPALabel.text
        
        // Unwrap optional
        guard let IPAStringRaw: String = IPAStringOptional as? String else {
            return
        }
        
        // Prevent the label from displaying "IPA" twice in the line and storing the wrong data
        let IPAString = IPAStringRaw.replacingOccurrences(of: "IPA: ", with: "")
        
        
        // Create link to VC3
        guard let VC = storyboard?.instantiateViewController(identifier: "VC3") as? ViewController3 else {
            return
        }
        
        // Present (go to) VC3
        VC.modalPresentationStyle = .fullScreen
        present(VC, animated: true)
        
        
        
        // Post notification of inputted IP Address for viewing
        NotificationCenter.default.post(name: Notification.Name("IPAin"), object: IPAString)
          
    }
    



}
