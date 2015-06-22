; This rule creates a relative clause in which the modified noun is the object of the verb in the clause, such as in
; "the man you sent the email to" or "the man wo whoom you sent the email"
; (June 2015 AN)

(define relmodiobj
    (BindLink
        (VariableList
            (TypedVariableLink
                (VariableNode "$a-parse")
                (TypeNode "ParseNode")
            )
            (TypedVariableLink
                (VariableNode "$subj")
                (TypeNode "WordInstanceNode")
            )
            (TypedVariableLink
                (VariableNode "$obj")
                (TypeNode "WordInstanceNode")
            )
            (TypedVariableLink
                (VariableNode "$rel")
                (TypeNode "WordInstanceNode")
            )
	    (TypedVariableLink
                (VariableNode "$pred")
                (TypeNode "WordInstanceNode")
            )
	    (TypedVariableLink
                (VariableNode "$iobj")
                (TypeNode "WordInstanceNode")
            )
        )
        (AndLink
            (WordInstanceLink
                (VariableNode "$obj")
                (VariableNode "$a-parse")
            )
            (WordInstanceLink
                (VariableNode "$rel")
                (VariableNode "$a-parse")
            )
            (WordInstanceLink
                (VariableNode "$subj")
                (VariableNode "$a-parse")
	    )
            (WordInstanceLink
                (VariableNode "$pred")
                (VariableNode "$a-parse")
	    )
	    (WordInstanceLink
                (VariableNode "$iobj")
                (VariableNode "$a-parse")
	    )
            (EvaluationLink
                (DefinedLinguisticRelationshipNode "_relmod")
                (ListLink
                    	(VariableNode "$iobj")
			(VariableNode "$rel")     
                )
            )
	    (EvaluationLink
                (DefinedLinguisticRelationshipNode "_iobj")
                (ListLink
                    	(VariableNode "$pred")
			(VariableNode "$iobj")     
                )
            )
        )
        (ExecutionOutputLink
       	   (GroundedSchemaNode "scm: pre-relmodiobj-rule")
       	      (ListLink
       	         (VariableNode "$obj")
		 (VariableNode "$rel")
		 (VariableNode "$subj")
		 (VariableNode "$pred")
		 (VariableNode "$iobj")       
            )
        )
    )
)

(InheritanceLink (stv 1 .99) (ConceptNode "relmodiobj-Rule") (ConceptNode "Rule"))

(ReferenceLink (stv 1 .99) (ConceptNode "relmodiobj-Rule") relmodiobj)

; This is function is not needed. It is added so as not to break the existing
; r2l pipeline.
(define (pre-relmodiobj-rule iobj rel subj pred obj)
    	(relmodiobj-rule (word-inst-get-word-str iobj) (cog-name iobj)
		(word-inst-get-word-str rel) (cog-name rel)  
		(word-inst-get-word-str subj) (cog-name subj)
		(word-inst-get-word-str pred) (cog-name pred)
	        (word-inst-get-word-str obj) (cog-name obj)                
	)
)

