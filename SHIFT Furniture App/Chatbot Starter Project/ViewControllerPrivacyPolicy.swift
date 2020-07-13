//
//  ViewControllerPrivacyPolicy.swift
//  SHIFT Furniture
//
//  Created by Kyle Skyllingstad on 6/11/20.
//

import UIKit
import WebKit

// Class for the Privacy Policy screen
class ViewControllerPrivacyPolicy: UIViewController {


    // Outlets for entities
    
    // WebView
    @IBOutlet weak var webView: WKWebView!
    

    // Main screen loading function
    override func viewDidLoad() {
        super.viewDidLoad()

        // Do any additional setup after loading the view.
        
        // Create local URL to call local Privacy Policy html file
        if let indexURL = Bundle.main.url(forResource: "PrivacyPolicyCode", withExtension: "html") {
                    
            // Load this local Privacy Policy html file into the WebView object
            self.webView.loadFileURL(indexURL, allowingReadAccessTo: indexURL)
        }
    
        
    }
    

    
    
    
    // Function for if back button from this screen (VC2PP) back to screen 2 (VC2) tapped
    @IBAction func didTapButtonPPto2() {
        
        // Create link to screen 2, VC2
        guard let VC = storyboard?.instantiateViewController(identifier: "VC2") as? ViewController2 else {
            return
        }
        
        // Present (go to) VC2
        VC.modalPresentationStyle = .fullScreen
        present(VC, animated: false)
    }
    
    
    

}
